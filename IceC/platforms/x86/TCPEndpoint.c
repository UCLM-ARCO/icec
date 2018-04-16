/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceUtil.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#ifndef MAX_TCP_ENDPOINT_INSTANCES
#define MAX_TCP_ENDPOINT_INSTANCES 5
#endif

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static TCP_EndpointOptions ep_options[MAX_TCP_ENDPOINT_INSTANCES];

int
getFD(Ice_EndpointInfoPtr self) {
  TCP_EndpointOptionsPtr opts = self->options;
  return opts->fd;
}

bool
TCP_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (! streq(proto, "tcp"))
    	return false;

    *result = Ice_EndpointTypeTCP;
    return true;
}

bool
TCP_InputStream_init(Ice_InputStreamPtr self, Ice_ConnectionPtr connection, Ice_EndpointType type) {
    int fd;
    
    trace();
    
    if (type != Ice_EndpointTypeTCP)
	return false;

    TCP_EndpointOptionsPtr opts = connection->epinfo->options;
    if (opts->current_client != -1)
      fd = opts->current_client;
    else
      fd = opts->fd;
    self->size = read(fd, (byte*)self->data, MAX_MESSAGE_SIZE);
    return true;
}

bool
TCP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    TCP_EndpointOptions* opts;

    trace();

    if (self->epinfo.type != Ice_EndpointTypeTCP)
	return false;

    opts = self->epinfo.options;
    Ptr_check(opts);

    return TCP_Connection_listen(&(self->epinfo), opts->host, opts->port);
}

void ICEC_FUNC_ATTR
_get_options_slot(TCP_EndpointOptionsPtr* options) {
    byte i;

    *options = NULL;
    for (i=0; i<MAX_TCP_ENDPOINT_INSTANCES; i++) {
	if (ep_options[i].used)
	    continue;
	*options = &ep_options[i];
	break;
    }

    assert(*options != NULL && "Max number of endpoints reached!");
}

bool
TCP_EndpointInfo_init(Ice_EndpointInfoPtr self,
		      Ice_EndpointType type,
		      const char* str_endpoint) {
    TCP_EndpointOptionsPtr options;

    trace();
    if (type != Ice_EndpointTypeTCP)
	return false;

    _get_options_slot(&options);
    Ptr_check(options);

    self->type = type;
    self->datagram = false;
    self->options = options;
    options->used = true;

    IceUtil_getHost(str_endpoint, options->host);
    IceUtil_getPort(str_endpoint, &(options->port));
    Object_init((ObjectPtr)self);
    return true;
}

/*
  TCP proxy marshalling (inside an encap), version 1.0:
  - Identity (name + category: string)             |
  - facet: stringSeq				   |
  - mode: byte (0:toway, 1:oneway, 3:datagram)	   |
  - secure: bool				   |-> done by IceC
  - number of endpoints: size			   |
  - for each endpoint:				   |
      - type: short (1:tcp, 2:ssl, 3:udp...)       |

      - encap header (int size + version)          |
      - host: string				   |
      - port: int				   |-> must be done here
      - timeout: int				   |
      - compress: bool                             |
*/

bool
TCP_EndpointInfo_writeToOutputStream(Ice_EndpointInfoPtr self, Ice_OutputStreamPtr os) {
    TCP_EndpointOptionsPtr opts;
    trace();

    Ptr_check(self);
    if (self->type != Ice_EndpointTypeTCP)
	return false;

    opts = self->options;
    Ptr_check(opts);
    Ptr_check(os);

    Ice_OutputStream_startWriteEncaps(os);
    Ice_OutputStream_writeString(os, opts->host);
    Ice_OutputStream_writeInt(os, opts->port);
    Ice_OutputStream_writeInt(os, opts->timeout);
    Ice_OutputStream_writeBool(os, false);
    Ice_OutputStream_endWriteEncaps(os);

    return true;
}

bool
TCP_EndpointInfo_readFromInputStream(Ice_EndpointInfoPtr self, Ice_InputStreamPtr is) {
    TCP_EndpointOptionsPtr opts;
    Ice_Int encapSize = 0;
    byte major = 0;
    byte minor = 0;
    trace();

    /* Note: type is set previously by IceC engine */
    if (self->type != Ice_EndpointTypeTCP)
	return false;

    _get_options_slot(&opts);
    self->options = opts;
    Ptr_check(opts);
    Ptr_check(is);

    self->datagram = false;

    /* read and check encapsulation header (note that this encap is
       nested, may be more data after it) */
    Ice_InputStream_readInt(is, &encapSize);
    Ice_InputStream_readByte(is, &major);
    Ice_InputStream_readByte(is, &minor);
    assert(major == 1 && minor == 0);
    assert(encapSize - 6 <= (is->size - (is->next - is->data)));

    /* read other endpoint params */
    Ice_InputStream_readString(is, opts->host);
    Ice_InputStream_readInt(is, (Ice_Int*)&(opts->port));
    Ice_InputStream_readInt(is, (Ice_Int*)&(opts->timeout));

    /* Note: compression not supported */
    {
	bool comp;
	Ice_InputStream_readBool(is, &comp);
    }

    Object_init((ObjectPtr)self);
    return true;
}

bool
TCP_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    TCP_EndpointOptionsPtr opts;

    trace();
    Ptr_check(self);

    if (self->epinfo.type != Ice_EndpointTypeTCP)
	return false;

    opts = self->epinfo.options;
    Ptr_check(opts);

    TCP_Connection_connect(&(self->connection), opts->host, opts->port);
    //assert(self->connection.fd != -1);
    return true;
}

bool
TCP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx,
		   IcePlugin_EndpointObjectPtr endp) {
    trace();
    if (type != Ice_EndpointTypeTCP)
	return false;

    Ice_Connection_init(&(self->connection), &(self->epinfo), endp);
    Ice_OutputStream_init(&(self->stream));
    TCP_EndpointInfo_init(&(self->epinfo), type, strprx);

    Object_init((ObjectPtr)self);
    return true;
}

void
TCP_Connection_connect(Ice_ConnectionPtr self, const char* host, uint16_t port) {
    struct sockaddr_in addr;

    trace();

    memset(&addr, 0, sizeof(struct sockaddr_in));

    TCP_EndpointOptionsPtr opts = self->epinfo->options;
    opts->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (opts->fd == -1) {
	error("Could not create socket: %s\n", strerror(errno));
	return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (! inet_aton(host, &(addr.sin_addr))) {
	error("Invalid host name: %s\n", host);
        return;
    }

    if (connect(opts->fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	error("Could not bind socket: %s\n", strerror(errno));
	return;
    }
}

bool
TCP_Connection_listen(Ice_EndpointInfoPtr self, const char* host, uint16_t port) {
    struct sockaddr_in addr;
    int optval;

    trace();

    memset(&addr, 0, sizeof(struct sockaddr_in));

    TCP_EndpointOptions* opts = self->options;
    opts->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (opts->fd == -1) {
	error("Could not create socket: %s\n", strerror(errno));
	return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (! inet_aton(host, &(addr.sin_addr))) {
	error("Invalid host name: %s\n", host);
        return false;
    }

    optval = 1;
    setsockopt(opts->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    if (bind(opts->fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	error("Could not bind socket: %s\n", strerror(errno));
	return false;
    }

    listen(opts->fd, 1);
    return true;
}

bool
TCP_Connection_accept(Ice_ConnectionPtr self) {
    trace();
    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;

    TCP_EndpointOptions* opts = self->epinfo->options;
    opts->current_client = accept(opts->fd, NULL, 0);
    return true;
}

bool
TCP_Connection_send(Ice_ConnectionPtr self, byte* data, uint16_t* size) {
    int fd;
  
    trace();
    
    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;

    TCP_EndpointOptionsPtr opts = self->epinfo->options;
    if (opts->current_client != -1)
        fd = opts->current_client;
    else
        fd = opts->fd;
    *size = send(fd, data, *size, 0);
    return true;
}

bool
TCP_Connection_close(Ice_ConnectionPtr self) {
    trace();
    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;
    TCP_EndpointOptionsPtr opts = self->epinfo->options;
    close(opts->current_client);
    opts->current_client = -1;
    return true;
}

bool
TCP_Connection_dataReady(Ice_ConnectionPtr self, bool* result) {
    fd_set read_set;
    uint16_t i = 0;
    struct timeval tv;

    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;

    TCP_EndpointOptionsPtr opts = self->epinfo->options;
    FD_ZERO(&read_set);
    FD_SET(opts->fd + 1, &read_set);

    /* Wait up to two seconds. */
    for (i=0; i<2000; i++) {
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	if (select(1, &read_set, NULL, NULL, &tv) < 0)
	    continue;

	*result = true;
	return true;
    }

    error("Error while reading from descriptors: %s\n", strerror(errno));
    return false;
}

void
TCPEndpoint_init(Ice_CommunicatorPtr ic) {
    byte i;
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    for (i=0; i<MAX_TCP_ENDPOINT_INSTANCES; i++) {
	ep_options[i].used = false;
	ep_options[i].fd = -1;
	ep_options[i].current_client = -1;		
	//ep_options[i].ep_index = i;
	ep_options[i].timeout = TCP_DEFAULT_TIMEOUT;
	Object_init((ObjectPtr)&ep_options[i]);
    }

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

    endpoint.getProtocolType = &TCP_getProtocolType;
    endpoint.InputStream_init = &TCP_InputStream_init;
    endpoint.ObjectAdapter_activate = &TCP_ObjectAdapter_activate;
    endpoint.EndpointInfo_init = &TCP_EndpointInfo_init;
    endpoint.EndpointInfo_writeToOutputStream = &TCP_EndpointInfo_writeToOutputStream;
    endpoint.EndpointInfo_readFromInputStream = &TCP_EndpointInfo_readFromInputStream;
    endpoint.ObjectPrx_init = &TCP_ObjectPrx_init;
    endpoint.ObjectPrx_connect = &TCP_ObjectPrx_connect;
    endpoint.Connection_accept = &TCP_Connection_accept;
    endpoint.Connection_send = &TCP_Connection_send;
    endpoint.Connection_close = &TCP_Connection_close;
    endpoint.Connection_dataReady = &TCP_Connection_dataReady;
}

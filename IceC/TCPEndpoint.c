/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/TCPEndpoint.h>
#include <IceC/IceUtil.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

/* NOTE: only one endpoint instance allowed */
static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static TCP_EndpointOptions ep_options;

bool
TCP_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (! streq(proto, "tcp"))
    	return false;

    *result = Ice_EndpointTypeTCP;
    return true;
}

bool
TCP_InputStream_init(Ice_InputStreamPtr this, int fd, Ice_EndpointType type) {
    trace();
    if (type != Ice_EndpointTypeTCP)
	return false;

    this->size = read(fd, (byte*)this->data, MAX_MESSAGE_SIZE);
    return true;
}

bool
TCP_ObjectAdapter_activate(Ice_ObjectAdapterPtr this) {
    TCP_EndpointOptions* opts;

    trace();

    if (this->epinfo.type != Ice_EndpointTypeTCP)
	return false;

    opts = this->epinfo.options;
    Ptr_check(opts);

    return TCP_Connection_listen(&(this->connection), opts->host, opts->port);
}

bool
TCP_EndpointInfo_init(Ice_EndpointInfoPtr this,
		      Ice_EndpointType type,
		      const char* endpoint) {

    trace();
    if (type != Ice_EndpointTypeTCP)
	return false;

    Ptr_check(&ep_options);
    assert(! ep_options.used && "Only one instance of this endpoint allowed!");

    this->type = type;
    this->datagram = false;
    this->options = &ep_options;
    ep_options.used = true;

    IceUtil_getHost(endpoint, ep_options.host);
    IceUtil_getPort(endpoint, &(ep_options.port));
    Object_init((ObjectPtr)this);
    return true;
}

bool
TCP_ObjectPrx_connect(Ice_ObjectPrxPtr this) {
    fixme("not implemented");
    return false;
}

bool
TCP_ObjectPrx_init(Ice_ObjectPrxPtr this, const char* id,
		   Ice_EndpointType type, const char* strprx) {
    fixme("not implemented");
    return false;
}

bool
TCP_Connection_listen(Ice_ConnectionPtr this, const char* host, uint16_t port) {
    struct sockaddr_in addr;
    int optval;

    trace();

    memset(&addr, 0, sizeof(struct sockaddr_in));

    this->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->fd == -1) {
	error("Could not create socoket: %s\n", strerror(errno));
	return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (! inet_aton(host, &(addr.sin_addr))) {
	error("Invalid host name: %s\n", host);
        return false;
    }

    optval = 1;
    setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    if (bind(this->fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	error("Could not bind socket: %s\n", strerror(errno));
	return false;
    }

    listen(this->fd, 1);
    return true;
}

bool
TCP_Connection_accept(Ice_ConnectionPtr this) {
    trace();
    if (this->type != Ice_EndpointTypeTCP)
	return false;

    this->current_client = accept(this->fd, NULL, 0);
    return true;
}

bool
TCP_Connection_send(Ice_ConnectionPtr this, int fd, byte* data, uint16_t* size) {
    trace();
    if (this->type != Ice_EndpointTypeTCP)
	return false;

    *size = send(fd, data, *size, 0);
    return true;
}

bool
TCP_Connection_close(Ice_ConnectionPtr this) {
    trace();
    if (this->type != Ice_EndpointTypeTCP)
	return false;

    close(this->current_client);
    return true;
}

void
TCPEndpoint_init(Ice_CommunicatorPtr ic) {
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    ep_options.used = false;
    Object_init((ObjectPtr)&ep_options);

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

    endpoint.getProtocolType = &TCP_getProtocolType;
    endpoint.InputStream_init = &TCP_InputStream_init;
    endpoint.ObjectAdapter_activate = &TCP_ObjectAdapter_activate;
    endpoint.EndpointInfo_init = &TCP_EndpointInfo_init;
    endpoint.ObjectPrx_connect = &TCP_ObjectPrx_connect;
    endpoint.ObjectPrx_init = &TCP_ObjectPrx_init;
    endpoint.Connection_accept = &TCP_Connection_accept;
    endpoint.Connection_send = &TCP_Connection_send;
    endpoint.Connection_close = &TCP_Connection_close;
}

/* -*- mode: c++; coding: utf-8 -*- */

#include <ESP8266WiFi.h>
#include <IceC/IceUtil.h>
#include <IceC/platforms/esp8266/TCPEndpoint.h>

#ifndef MAX_TCP_ENDPOINT_INSTANCES
#define MAX_TCP_ENDPOINT_INSTANCES 3
#endif

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static TCP_EndpointOptions ep_options[MAX_TCP_ENDPOINT_INSTANCES];

bool ICEC_FUNC_ATTR
TCP_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (! streq(proto, "tcp"))
    	return false;

    *result = Ice_EndpointTypeTCP;
    return true;
}

bool ICEC_FUNC_ATTR
TCP_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) {
    trace();

    if (type != Ice_EndpointTypeTCP)
	return false;

    self->size = ep_options[fd].client->read((byte*)self->data, MAX_MESSAGE_SIZE);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    TCP_EndpointOptions* opts;

    trace();
    fixme("Not implemented!\n");

/*     if (self->epinfo.type != Ice_EndpointTypeTCP) */
/* 	return false; */

/*     opts = self->epinfo.options; */
/*     Ptr_check(opts); */

/*     return TCP_Connection_listen(&(self->connection), opts->host, opts->port); */
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

bool ICEC_FUNC_ATTR
TCP_EndpointInfo_init(Ice_EndpointInfoPtr self,
		      Ice_EndpointType type,
		      const char* endpoint) {
    ObjectPtr ptr;

    trace();

    if (type != Ice_EndpointTypeTCP)
	return false;

    TCP_EndpointOptionsPtr options;
    _get_options_slot(&options);
    Ptr_check(options);

    self->type = type;
    self->datagram = false;
    self->options = options;
    options->used = true;

    IceUtil_getHost(endpoint, options->host);
    IceUtil_getPort(endpoint, &(options->port));
    Object_init((ObjectPtr)self);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    TCP_EndpointOptionsPtr opts;

    trace();
    Ptr_check(self);

    if (self->epinfo.type != Ice_EndpointTypeTCP)
	return false;

    opts = (TCP_EndpointOptionsPtr)self->epinfo.options;
    Ptr_check(opts);

    TCP_Connection_connect(&(self->connection), opts->host, opts->port);
    assert(self->connection.fd != -1);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx) {
    trace();

    if (type != Ice_EndpointTypeTCP)
	return false;

    Ice_Connection_init(&(self->connection), &(self->epinfo));
    Ice_OutputStream_init(&(self->stream));
    TCP_EndpointInfo_init(&(self->epinfo), type, strprx);

    Object_init((ObjectPtr)self);
    return true;
}

void ICEC_FUNC_ATTR
TCP_Connection_connect(Ice_ConnectionPtr self, const char* host, uint16_t port) {
    TCP_EndpointOptionsPtr opts;

    trace();

    opts = (TCP_EndpointOptionsPtr)self->epinfo->options;
    Ptr_check(opts);

    assert(WiFi.status() == WL_CONNECTED && "WiFi not connected");
    assert(opts->client == NULL && "Already used endpoint");

    opts->client = new WiFiClient;
    bool result = opts->client->connect(host, port);
    assert(result && "Connection failed -1");
    self->fd = opts->fd;
}

bool ICEC_FUNC_ATTR
TCP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port) {
    /* struct sockaddr_in addr; */
    int optval;

    trace();
    fixme("Not implemented!\n");


/*     memset(&addr, 0, sizeof(struct sockaddr_in)); */

/*     self->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); */
/*     if (self->fd == -1) { */
/* 	error("Could not create socket: %s\n", strerror(errno)); */
/* 	return false; */
/*     } */

/*     addr.sin_family = AF_INET; */
/*     addr.sin_port = htons(port); */

/*     if (! inet_aton(host, &(addr.sin_addr))) { */
/* 	error("Invalid host name: %s\n", host); */
/*         return false; */
/*     } */

/*     optval = 1; */
/*     setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval); */
/*     if (bind(self->fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) { */
/* 	error("Could not bind socket: %s\n", strerror(errno)); */
/* 	return false; */
/*     } */

/*     listen(self->fd, 1); */
/*     return true; */
}

bool ICEC_FUNC_ATTR
TCP_Connection_accept(Ice_ConnectionPtr self) {
    trace();
    fixme("Not implemented!\n");

/*     if (self->type != Ice_EndpointTypeTCP) */
/* 	return false; */

/*     self->current_client = accept(self->fd, NULL, 0); */
/*     return true; */
}

bool ICEC_FUNC_ATTR
TCP_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t* size) {
    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;

    *size = ep_options[fd].client->write((const uint8_t*)data, *size);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_close(Ice_ConnectionPtr self) {
    trace();
    fixme("Not implemented!\n");

/*     if (self->type != Ice_EndpointTypeTCP) */
/* 	return false; */

/*     close(self->current_client); */
/*     return true; */
}

void ICEC_FUNC_ATTR
TCPEndpoint_init(Ice_CommunicatorPtr ic) {
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    for (byte i=0; i<MAX_TCP_ENDPOINT_INSTANCES; i++) {
	ep_options[i].fd = i;
	ep_options[i].used = false;
	ep_options[i].client = NULL;
	Object_init((ObjectPtr)&ep_options[i]);
    }

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

    // get a (almost) ramdom number for source port
    int sport = ESP.getCycleCount() % 32000 + 32000;
    WiFiClient::setLocalPortStart(sport);

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

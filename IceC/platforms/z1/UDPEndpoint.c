/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceUtil.h>
#include <IceC/platforms/z1/UDPEndpoint.h>

#include <net/uip-udp-packet.h>

#ifndef MAX_UDP_ENDPOINT_INSTANCES
#define MAX_UDP_ENDPOINT_INSTANCES 3
#endif

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static UDP_EndpointOptions ep_options[MAX_UDP_ENDPOINT_INSTANCES];

bool
UDP_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (! streq(proto, "udp"))
    	return false;

    *result = Ice_EndpointTypeUDP;
    return true;
}

/* bool */
/* UDP_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) { */
/*     trace(); */
/*     if (type != Ice_EndpointTypeUDP) */
/* 	return false; */

/*     self->size = read(fd, (byte*)self->data, MAX_MESSAGE_SIZE); */
/*     return true; */
/* } */

/* bool */
/* UDP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) { */
/*     UDP_EndpointOptionsPtr opts; */

/*     trace(); */

/*     if (self->epinfo.type != Ice_EndpointTypeUDP) */
/* 	return false; */

/*     opts = self->epinfo.options; */
/*     Ptr_check(opts); */

/*     return UDP_Connection_listen(&(self->connection), opts->host, opts->port); */
/* } */

void
UDP_getIPv6Host(const char* s, uip_ipaddr_t* addr) {
    char str_addr[40];
    IceUtil_getHost(s, str_addr);

    /* addr is like: 0000:0000:0000:0000:0000:0000:0000:0000 */
    byte i;
    for (i=0; i<8; i++)
	addr->u16[i] = strtol(str_addr + 5*i, NULL, 16);
}

bool
UDP_EndpointInfo_init(Ice_EndpointInfoPtr self,
		      Ice_EndpointType type,
		      const char* endpoint) {
    trace();

    if (type != Ice_EndpointTypeUDP)
	return false;

    UDP_EndpointOptions* options = NULL;
    byte i;
    for (i=0; i<MAX_UDP_ENDPOINT_INSTANCES; i++) {
    	if (ep_options[i].used)
	    continue;
    	options = &ep_options[i];
	break;
    }

    assert(options != NULL && "Max number of endpoints reached!");
    Ptr_check(options);

    self->type = type;
    self->datagram = true;
    self->options = options;
    options->used = true;
    options->uip_connection = NULL;

    UDP_getIPv6Host(endpoint, &options->host);
    IceUtil_getPort(endpoint, &options->port);
    options->port = UIP_HTONS(options->port);
    Object_init((ObjectPtr)self);
    return true;
}

bool
UDP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx) {
    trace();

    if (type != Ice_EndpointTypeUDP)
	return false;

    Ice_Connection_init(&(self->connection), &(self->epinfo));
    Ice_OutputStream_init(&(self->stream));
    UDP_EndpointInfo_init(&(self->epinfo), type, strprx);

    self->mode = Ice_ReferenceModeDatagram;
    Object_init((ObjectPtr)self);
    return true;
}

bool
UDP_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    UDP_EndpointOptionsPtr options;

    trace();
    if (self->epinfo.type != Ice_EndpointTypeUDP)
	return false;

    options = self->epinfo.options;
    options->uip_connection = uip_udp_new(&options->host, options->port);
    if (options->uip_connection == NULL)
    	throw("Could not create connection", __FILE__, __LINE__);

    self->connection.fd = 0;
    return true;
}

/* bool */
/* UDP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port) { */
/*     struct sockaddr_in addr; */
/*     int optval; */

/*     trace(); */

/*     memset(&addr, 0, sizeof(struct sockaddr_in)); */

/*     self->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); */
/*     if (self->fd == -1) { */
/* 	error("Could not create socoket: %s\n", strerror(errno)); */
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
/* } */

bool
UDP_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t* size) {
    trace();
    if (self->epinfo->type != Ice_EndpointTypeUDP)
	return false;

    UDP_EndpointOptionsPtr options = self->epinfo->options;
    uip_udp_packet_sendto(options->uip_connection, data, *size,
			  &options->host, options->port);
    return true;
}

void
UDPEndpoint_init(Ice_CommunicatorPtr ic) {
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    byte i;
    for (i=0; i<MAX_UDP_ENDPOINT_INSTANCES; i++) {
	ep_options[i].used = false;
	Object_init((ObjectPtr)&ep_options[i]);
    }

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

    endpoint.getProtocolType = &UDP_getProtocolType;

    /*     endpoint.InputStream_init = &UDP_InputStream_init; */
    endpoint.InputStream_init = NULL;

    /*     endpoint.ObjectAdapter_activate = &UDP_ObjectAdapter_activate; */
    endpoint.ObjectAdapter_activate = NULL;

    endpoint.EndpointInfo_writeToOutputStream = NULL;
    endpoint.EndpointInfo_init = &UDP_EndpointInfo_init;
    endpoint.ObjectPrx_connect = &UDP_ObjectPrx_connect;
    endpoint.ObjectPrx_init = &UDP_ObjectPrx_init;

    /*     endpoint.Connection_accept = NULL; */
    endpoint.Connection_accept = NULL;

    endpoint.Connection_send = &UDP_Connection_send;

    /*     endpoint.Connection_close = NULL; */
    endpoint.Connection_close = NULL;

    endpoint.Connection_dataReady = NULL;
}

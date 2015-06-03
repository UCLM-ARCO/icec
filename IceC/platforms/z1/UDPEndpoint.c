/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/platforms/z1/UDPEndpoint.h>
#include <IceC/IceUtil.h>

/* #include <netinet/in.h> */
/* #include <sys/socket.h> */
/* #include <arpa/inet.h> */

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

/* bool */
/* UDP_EndpointInfo_init(Ice_EndpointInfoPtr self, */
/* 		      Ice_EndpointType type, */
/* 		      const char* endpoint) { */

/*     ObjectPtr _ep_options_ptr; */

/*     trace(); */
/*     if (type != Ice_EndpointTypeUDP) */
/* 	return false; */

/*     /\* avoid annoying compiler messages *\/ */
/*     _ep_options_ptr = (ObjectPtr)&ep_options; */
/*     Ptr_check(_ep_options_ptr); */

/*     assert(! ep_options.used && "Only one instance of this endpoint allowed!"); */

/*     self->type = type; */
/*     self->datagram = true; */
/*     self->options = &ep_options; */
/*     ep_options.used = true; */

/*     IceUtil_getHost(endpoint, ep_options.host); */
/*     IceUtil_getPort(endpoint, &(ep_options.port)); */
/*     Object_init((ObjectPtr)self); */
/*     return true; */
/* } */

/* bool */
/* UDP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx) { */
/*     ObjectPtr _ep_options_ptr; */

/*     trace(); */
/*     if (type != Ice_EndpointTypeUDP) */
/* 	return false; */

/*     /\* avoid annoying compiler messages *\/ */
/*     _ep_options_ptr = (ObjectPtr)&ep_options; */
/*     Ptr_check(_ep_options_ptr); */

/*     assert(! ep_options.used && "Only one instance of this endpoint allowed!"); */

/*     Ice_Connection_init(&(self->connection), &(self->epinfo)); */
/*     Ice_OutputStream_init(&(self->stream)); */
/*     UDP_EndpointInfo_init(&(self->epinfo), type, strprx); */

/*     self->mode = Ice_ReferenceModeDatagram; */
/*     Object_init((ObjectPtr)self); */
/*     return true; */
/* } */

/* bool */
/* UDP_ObjectPrx_connect(Ice_ObjectPrxPtr self) { */
/*     Ice_ConnectionPtr conn; */
/*     UDP_EndpointOptionsPtr opts; */
/*     struct sockaddr_in addr; */
/*     int res; */

/*     trace(); */
/*     if (self->epinfo.type != Ice_EndpointTypeUDP) */
/* 	return false; */

/*     conn = &(self->connection); */
/*     Ptr_check(conn); */
/*     opts = self->epinfo.options; */
/*     Ptr_check(opts); */

/*     memset(&addr, 0, sizeof(struct sockaddr_in)); */

/*     conn->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); */
/*     if (conn->fd == -1) { */
/*     	error("Could not create socoket: %s\n", strerror(errno)); */
/*     	return false; */
/*     } */

/*     addr.sin_family = AF_INET; */
/*     addr.sin_port = htons(opts->port); */

/*     if (! inet_aton(opts->host, &(addr.sin_addr))) { */
/*     	error("Invalid host name: %s\n", opts->host); */
/*         return false; */
/*     } */

/*     res = connect(conn->fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)); */
/*     if (res == -1) { */
/*     	error("Could not connect: %s\n", strerror(errno)); */
/*     	return false; */
/*     } */

/*     return true; */
/* } */

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

/* bool */
/* UDP_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t* size) { */
/*     trace(); */
/*     if (self->epinfo->type != Ice_EndpointTypeUDP) */
/* 	return false; */

/*     *size = send(fd, data, *size, 0); */
/*     return true; */
/* } */

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
/*     endpoint.ObjectAdapter_activate = &UDP_ObjectAdapter_activate; */
/*     endpoint.EndpointInfo_init = &UDP_EndpointInfo_init; */
/*     endpoint.ObjectPrx_connect = &UDP_ObjectPrx_connect; */
/*     endpoint.ObjectPrx_init = &UDP_ObjectPrx_init; */
/*     endpoint.Connection_accept = NULL; */
/*     endpoint.Connection_send = &UDP_Connection_send; */
/*     endpoint.Connection_close = NULL; */

    endpoint.InputStream_init = NULL;
    endpoint.ObjectAdapter_activate = NULL;
    endpoint.EndpointInfo_init = NULL;
    endpoint.EndpointInfo_writeToOutputStream = NULL;
    endpoint.ObjectPrx_init = NULL;
    endpoint.ObjectPrx_connect = NULL;
    endpoint.Connection_accept = NULL;
    endpoint.Connection_send = NULL;
    endpoint.Connection_close = NULL;
    endpoint.Connection_dataReady = NULL;

}

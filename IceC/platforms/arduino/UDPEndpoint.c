/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceUtil.h>
#include <IceC/platforms/arduino/UDPEndpoint.h>
#include <IceC/platforms/arduino/UDPIO.h>

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static UDP_EndpointOptions ep_options;

bool
IceC_UDPEndpoint_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (!streq(proto, "udp"))
    	return false;

    *result = Ice_EndpointTypeUDP;
    return true;
}

bool
IceC_UDPEndpoint_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) {
    uint32_t size = 0;

    trace();

    if (type != Ice_EndpointTypeUDP)
	return false;

    /* first: read header, and get message size */
    self->size = UDP_read((byte*)self->data, 14);
    size = (uint32_t)*(self->data + 10);

    /* then: read as size specifies */
    size -= 14;
    assert(size <= MAX_MESSAGE_SIZE && "Message too big!");

    self->size += UDP_read((byte*)(self->data + 14), size);
    return true;
}

bool
IceC_UDPEndpoint_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    UDP_EndpointOptionsPtr opts;

    trace();

    if (self->epinfo.type != Ice_EndpointTypeUDP)
	return false;

    opts = self->epinfo.options;
    Ptr_check(opts);

    self->connection.fd = UDP_init(opts->port);
    return true;
}

bool
IceC_UDPEndpoint_EndpointInfo_init(Ice_EndpointInfoPtr self,
				   Ice_EndpointType type,
				   const char* endpoint) {
    trace();

    if (type != Ice_EndpointTypeUDP)
	return false;

    self->type = type;
    self->datagram = true;
    self->options = &ep_options;

    IceUtil_getPort(endpoint, &(ep_options.port));
    Object_init((ObjectPtr)self);
    return true;
}

bool
IceC_UDPEndpoint_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    trace();
    if (self->epinfo.type != Ice_EndpointTypeUDP)
	return false;

    throw("NotImplementedError", __FILE__, __LINE__);
    return false;
}

bool
IceC_UDPEndpoint_ObjectPrx_init(Ice_ObjectPrxPtr self,
			   Ice_EndpointType type,
			   const char* strprx) {
    trace();

    if (self->epinfo.type != Ice_EndpointTypeUDP)
	return false;

    throw("NotImplementedError", __FILE__, __LINE__);
    return false;
}

bool
IceC_UDPEndpoint_Connection_send(Ice_ConnectionPtr self, int fd,
				 byte* data, uint16_t* size) {
    trace();

    if (self->type != Ice_EndpointTypeUDP)
	return false;

    throw("NotImplementedError", __FILE__, __LINE__);
    return false;
}

bool
IceC_UDPEndpoint_Connection_dataReady(Ice_ConnectionPtr self, bool* ready) {
    trace();
    if (self->type != Ice_EndpointTypeUDP)
	return false;

    *ready = UDP_available();
    return true;
}

/* plugin specific functions */
void
IceC_UDPEndpoint_init(Ice_CommunicatorPtr ic) {
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    Object_init((ObjectPtr)&ep_options);

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);
    Ethernet_setup();

    endpoint.getProtocolType = &IceC_UDPEndpoint_getProtocolType;
    endpoint.InputStream_init = &IceC_UDPEndpoint_InputStream_init;
    endpoint.ObjectAdapter_activate = &IceC_UDPEndpoint_ObjectAdapter_activate;
    endpoint.EndpointInfo_init = &IceC_UDPEndpoint_EndpointInfo_init;
    endpoint.ObjectPrx_init = &IceC_UDPEndpoint_ObjectPrx_init;
    endpoint.ObjectPrx_connect = &IceC_UDPEndpoint_ObjectPrx_connect;
    endpoint.Connection_accept = NULL;
    endpoint.Connection_send = &IceC_UDPEndpoint_Connection_send;
    endpoint.Connection_close = NULL;
    endpoint.Connection_dataReady = &IceC_UDPEndpoint_Connection_dataReady;
}


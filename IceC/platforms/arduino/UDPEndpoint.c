/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/platforms/arduino/UDPEndpoint.h>
#include <IceC/platforms/arduino/UDPIO.h>

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
    trace();

    if (self->epinfo.type != Ice_EndpointTypeUDP)
	return false;

    throw("NotImplementedError", __FILE__, __LINE__);
    return false;
}

bool
IceC_UDPEndpoint_EndpointInfo_init(Ice_EndpointInfoPtr
			      self, Ice_EndpointType type,
			      const char* endpoint) {
    trace();

    if (self->type != Ice_EndpointTypeUDP)
	return false;

    throw("NotImplementedError", __FILE__, __LINE__);
    return false;
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

/* plugin specific functions */
void
IceC_UDPEndpoint_init(Ice_CommunicatorPtr ic) {
    trace();

    Ptr_check(ic);

    throw("NotImplementedError", __FILE__, __LINE__);
}


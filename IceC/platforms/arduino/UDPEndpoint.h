/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_UDP_ENDPOINT_H_
#define _ICEC_UDP_ENDPOINT_H_

#include <IceC/IceC.h>

#ifdef __cplusplus
extern "C" {
#endif

#define Ice_EndpointTypeUDP 13

typedef struct UDP_EndpointOptions {
    Object _base;
    uint16_t port;
} UDP_EndpointOptions;

typedef UDP_EndpointOptions* UDP_EndpointOptionsPtr;

bool IceC_UDPEndpoint_getProtocolType(const char* proto, Ice_EndpointType* result);
bool IceC_UDPEndpoint_InputStream_init(Ice_InputStreamPtr self, int fd,
				       Ice_EndpointType type);
bool IceC_UDPEndpoint_ObjectAdapter_activate(Ice_ObjectAdapterPtr self);
bool IceC_UDPEndpoint_EndpointInfo_init(Ice_EndpointInfoPtr self,
				Ice_EndpointType type,
				const char* endpoint);

bool IceC_UDPEndpoint_ObjectPrx_connect(Ice_ObjectPrxPtr self);
bool IceC_UDPEndpoint_ObjectPrx_init(Ice_ObjectPrxPtr self,
			     Ice_EndpointType type,
			     const char* strprx);

bool IceC_UDPEndpoint_Connection_send(Ice_ConnectionPtr self, int fd,
				      byte* data, uint16_t* size);
bool IceC_UDPEndpoint_Connection_dataReady(Ice_ConnectionPtr self, bool* ready);

/* plugin specific functions */
void IceC_UDPEndpoint_init(Ice_CommunicatorPtr ic);

#ifdef __cplusplus
}
#endif

#endif /* _ICEC_UDP8_ENDPOINT_H_ */

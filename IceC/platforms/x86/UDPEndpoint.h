/* -*- mode: c++; coding: utf-8 -*- */

#ifndef _ICE_UDPENDPOINT_H_
#define _ICE_UDPENDPOINT_H_

#include <IceC/IceC.h>

#define Ice_EndpointTypeUDP   3
#define MAX_HOST_SIZE         16

typedef struct UDP_EndpointOptions {
    Object _base;

    char host[MAX_HOST_SIZE];
    uint16_t port;
    bool used;
} UDP_EndpointOptions;

typedef UDP_EndpointOptions* UDP_EndpointOptionsPtr;

bool UDP_getProtocolType(const char* proto, Ice_EndpointType* result);
bool UDP_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type);
bool UDP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self);
bool UDP_EndpointInfo_init(Ice_EndpointInfoPtr self,
			   Ice_EndpointType type,
			   const char* endpoint);

bool UDP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx);
bool UDP_ObjectPrx_connect(Ice_ObjectPrxPtr self);

bool UDP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port);
bool UDP_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t* size);

/* plugin specific functions */
void UDPEndpoint_init(Ice_CommunicatorPtr ic);

#endif /* _ICE_UDPENDPOINT_H_ */

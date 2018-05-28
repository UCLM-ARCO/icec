/* -*- mode: c++; coding: utf-8 -*- */

#ifndef _ICE_UDPENDPOINT_H_
#define _ICE_UDPENDPOINT_H_

#include <IceC/IceC.h>
#include "TCP_UDPEndpoint.h"

#define Ice_EndpointTypeUDP   3

typedef TCP_UDPEndpointOptions* UDP_EndpointOptionsPtr;
typedef TCP_UDPEndpointOptions UDP_EndpointOptions;

bool UDP_getProtocolType(const char* proto, Ice_EndpointType* result);
bool UDP_InputStream_init(Ice_InputStreamPtr self, Ice_ConnectionPtr connection, Ice_EndpointType type);
bool UDP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self);
bool UDP_EndpointInfo_init(Ice_EndpointInfoPtr self,
			   Ice_EndpointType type,
			   const char* endpoint);

bool UDP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx,
		   IcePlugin_EndpointObjectPtr endp);
bool UDP_ObjectPrx_connect(Ice_ObjectPrxPtr self);

bool UDP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port);
bool UDP_Connection_send(Ice_ConnectionPtr self, byte* data, uint16_t* size);

/* plugin specific functions */
void UDPEndpoint_init(Ice_CommunicatorPtr ic);

#endif /* _ICE_UDPENDPOINT_H_ */

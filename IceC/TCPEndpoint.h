/* -*- mode: c++; coding: utf-8 -*- */

#ifndef _ICE_TCPENDPOINT_H_
#define _ICE_TCPENDPOINT_H_

#include <IceC/IceC.h>

#define Ice_EndpointTypeTCP   1
#define MAX_HOST_SIZE         16

typedef struct TCP_EndpointOptions {
    Object _base;

    char host[MAX_HOST_SIZE];
    uint16_t port;
    bool used;
} TCP_EndpointOptions;

typedef TCP_EndpointOptions* TCP_EndpointOptionsPtr;

bool TCP_getProtocolType(const char* proto, Ice_EndpointType* result);
bool TCP_InputStream_init(Ice_InputStreamPtr this, int fd, Ice_EndpointType type);
bool TCP_ObjectAdapter_activate(Ice_ObjectAdapterPtr this);
bool TCP_EndpointInfo_init(Ice_EndpointInfoPtr this,
			   Ice_EndpointType type,
			   const char* endpoint);

bool TCP_ObjectPrx_connect(Ice_ObjectPrxPtr this);
bool TCP_ObjectPrx_init(Ice_ObjectPrxPtr this, Ice_EndpointType type, const char* strprx);

void TCP_Connection_connect(Ice_ConnectionPtr this, const char* host, uint16_t port);
bool TCP_Connection_listen(Ice_ConnectionPtr this, const char* host, uint16_t port);
bool TCP_Connection_accept(Ice_ConnectionPtr this);
bool TCP_Connection_send(Ice_ConnectionPtr this, int fd, byte* data, uint16_t* size);
bool TCP_Connection_close(Ice_ConnectionPtr this);

/* plugin specific functions */
void TCPEndpoint_init(Ice_CommunicatorPtr ic);

#endif /* _ICE_TCPENDPOINT_H_ */

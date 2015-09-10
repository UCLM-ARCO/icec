/* -*- mode: c++; coding: utf-8 -*- */

#ifndef _ICE_TCPENDPOINT_H_
#define _ICE_TCPENDPOINT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <IceC/IceC.h>

#define Ice_EndpointTypeTCP   1
#define MAX_HOST_SIZE         32

typedef struct TCP_EndpointOptions {
    Object _base;

    char host[MAX_HOST_SIZE];
    uint16_t port;
    bool used;
    byte fd;
    WiFiServer* server;
    WiFiClient* client;

} TCP_EndpointOptions;

typedef TCP_EndpointOptions* TCP_EndpointOptionsPtr;

bool TCP_getProtocolType(const char* proto, Ice_EndpointType* result);
bool TCP_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type);
bool TCP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self);
bool TCP_EndpointInfo_init(Ice_EndpointInfoPtr self,
			   Ice_EndpointType type,
			   const char* endpoint);

bool TCP_ObjectPrx_connect(Ice_ObjectPrxPtr self);
bool TCP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx);

void TCP_Connection_connect(Ice_ConnectionPtr self, const char* host, uint16_t port);
bool TCP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port);
bool TCP_Connection_accept(Ice_ConnectionPtr self);
bool TCP_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t* size);
bool TCP_Connection_close(Ice_ConnectionPtr self);
bool TCP_Connection_dataReady(Ice_ConnectionPtr self, bool* result);

/* plugin specific functions */
void TCPEndpoint_init(Ice_CommunicatorPtr ic);

#ifdef __cplusplus
}
#endif

#endif /* _ICE_TCPENDPOINT_H_ */

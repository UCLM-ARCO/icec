/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICE_UDP_ENDPOINT_H_
#define _ICE_UDP_ENDPOINT_H_

#include <IceC/IceC.h>
#include <net/uip.h>

#define Ice_EndpointTypeUDP   3

typedef struct UDP_EndpointOptions {
    Object _base;

    uip_ipaddr_t host;
    uint16_t port;
    bool used;
    struct uip_udp_conn* uip_connection;
} UDP_EndpointOptions;

typedef UDP_EndpointOptions* UDP_EndpointOptionsPtr;

/* Public API */
void UDPEndpoint_init(Ice_CommunicatorPtr ic);

#endif /* _ICE_UDP_ENDPOINT_H_ */

/* -*- mode: c++; coding: utf-8 -*- */

#ifndef _ICE_TCP_UDPENDPOINT_H_
#define _ICE_TCP_UDPENDPOINT_H_

#include <IceC/IceC.h>

#define MAX_HOST_SIZE         32

typedef struct TCP_UDPEndpointOptions {
    Object _base;

    char host[MAX_HOST_SIZE];
    uint16_t port;
    bool used;
    int fd;
    Socket_t socket;
    Socket_t connected_socket;
    SocketSet_t socket_set;
} TCP_UDPEndpointOptions;


#endif /* _ICE_TCP_UDPENDPOINT_H_ */

/* -*- mode: c++; coding: utf-8 -*- */

#ifndef _ICEC_UDP_IO_H_
#define _ICEC_UDP_IO_H_

#include <IceC/IceC.h>

#ifndef MAC_ADDRESS
# error "MAC address not defined"
#endif

#ifndef IP_ADDRESS
# error "IP address not defined"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void Ethernet_setup();
    uint8_t UDP_init(uint16_t port);
    uint8_t UDP_read(byte* data, uint8_t size);
    bool UDP_available();

#ifdef __cplusplus
}
#endif

#endif /* _ICEC_UDP_IO_H_ */

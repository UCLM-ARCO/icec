/* -*- mode: c++; coding: utf-8 -*- */

#ifndef _ICEC_UDP_IO_H_
#define _ICEC_UDP_IO_H_

#include <IceC/IceC.h>

#ifndef MAC_ADDRESS
# error "MAC address not defined"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    uint8_t UDP_read(byte* data, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* _ICEC_UDP_IO_H_ */

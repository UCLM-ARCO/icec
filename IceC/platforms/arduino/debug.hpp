/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_DEBUG_HPP_
#define _ICE_DEBUG_HPP_

#include <HardwareSerial.h>

#ifdef __cplusplus
extern "C" {
#endif

void
prints(const char* msg) {
    Serial.print(msg);
}

void
printi(const int v) {
    Serial.print(v);
}

#ifdef __cplusplus
}
#endif

#endif /* _ICE_DEBUG_HPP_ */

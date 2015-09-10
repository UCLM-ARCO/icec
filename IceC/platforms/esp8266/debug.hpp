/* -*- mode: c++; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_DEBUG_HPP_
#define _ICE_DEBUG_HPP_

#include <HardwareSerial.h>

#ifdef __cplusplus
extern "C" {
#endif

void
prints(const char* msg) {
    Serial.print(msg);
    Serial.flush();
}

void
printi(const int v) {
    Serial.print(v);
    Serial.flush();
}

int
printh(const int v) {
   return Serial.print(v, HEX);
}

#ifdef __cplusplus
}
#endif

#endif /* _ICE_DEBUG_HPP_ */

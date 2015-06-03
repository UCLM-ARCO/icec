/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_PLATFORM_H_
#define _ICE_PLATFORM_H_

#if defined(__i386) || defined(_M_IX86) || defined(__x86_64) ||	\
    defined(_M_X64) || defined(_M_IA64)
#   include <IceC/platforms/x86/platform.h>

#elif defined(ARDUINO)
#   include <IceC/platforms/arduino/platform.h>

#elif defined(ESP8266)
#   include <IceC/platforms/esp8266/platform.h>

#elif defined(CONTIKI_TARGET_Z1)
#   include <IceC/platforms/z1/platform.h>

#else
#   error "Unknown architecture"
#endif

#endif /* _ICE_PLATFORM_H_ */

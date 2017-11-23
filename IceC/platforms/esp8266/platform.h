/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_PLATFORM_ESP8266_H_
#define _ICE_PLATFORM_ESP8266_H_

#define ICE_LITTLE_ENDIAN 1

#if !defined(ESP32)
  #define ICEC_FUNC_ATTR __attribute__((section(".irom0.text")))
#else
  #define ICEC_FUNC_ATTR __attribute__((section(".iram1")))
#endif

#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#if !defined(ARDUINO_ARCH_ESP32)
  #include <osapi.h>
#endif

#include <IceC/platforms/esp8266/flags.h>
#include <IceC/platforms/esp8266/types.h>
#include <IceC/platforms/esp8266/debug.h>
#include <IceC/platforms/esp8266/assert.h>
#include <IceC/platforms/esp8266/storage.h>

#ifndef MAX_MESSAGE_SIZE
  #define MAX_MESSAGE_SIZE        128
#endif

#define MAX_IDENTITY_SIZE       32
#define MAX_NESTED_ENCAPS       3
#define MAX_NUM_OF_ADAPTERS     1
#define MAX_NUM_OF_SERVANTS     10
#define MAX_PROTO_SIZE          8
#define MAX_OPERATION_NAME_SIZE 16
#define MAX_EXCEPTION_SIZE      32

#endif /* _ICE_PLATFORM_ESP8266_H_ */

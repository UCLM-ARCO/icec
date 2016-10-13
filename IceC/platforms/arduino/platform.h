/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_PLATFORM_ARDUINO_H_
#define _ICE_PLATFORM_ARDUINO_H_

#define ICE_LITTLE_ENDIAN
#define ICEC_FUNC_ATTR

#include <stdlib.h>
#include <errno.h>

#include <IceC/platforms/arduino/flags.h>
#include <IceC/platforms/arduino/types.h>
#include <IceC/platforms/arduino/debug.h>
#include <IceC/platforms/arduino/assert.h>
#include <IceC/platforms/arduino/strings.h>
#include <IceC/platforms/arduino/storage.h>

#define MAX_MESSAGE_SIZE        128
#define MAX_IDENTITY_SIZE       8
#define MAX_NESTED_ENCAPS       3
#define MAX_NUM_OF_ADAPTERS     1
#define MAX_NUM_OF_SERVANTS     10
#define MAX_PROTO_SIZE          8
#define MAX_OPERATION_NAME_SIZE 16

#endif /* _ICE_PLATFORM_ARDUINO_H_ */

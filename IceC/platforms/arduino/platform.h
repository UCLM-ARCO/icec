/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_PLATFORM_ARDUINO_H_
#define _ICE_PLATFORM_ARDUINO_H_

#define ICE_LITTLE_ENDIAN
#define ICEC_FUNC_ATTR

#include <stdlib.h>
#include <errno.h>

#include <IceC/platforms/arduino/flags.h>

#ifndef ARDUINO_ARCH_SAMD
#include <IceC/platforms/arduino/types.h>
#else
#include <sys/types.h>
#endif

#include <IceC/platforms/arduino/assert.h>
#include <IceC/platforms/arduino/debug.h>
#include <IceC/platforms/arduino/strings.h>
#include <IceC/platforms/arduino/storage.h>

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE        128
#endif

#ifndef MAX_IDENTITY_SIZE
#define MAX_IDENTITY_SIZE       8
#endif

#ifndef MAX_ADAPTER_ID_SIZE
#define MAX_ADAPTER_ID_SIZE     32
#endif

#ifndef MAX_NESTED_ENCAPS
#define MAX_NESTED_ENCAPS       3
#endif

#ifndef MAX_NUM_OF_ADAPTERS
#define MAX_NUM_OF_ADAPTERS     1
#endif

#ifndef MAX_NUM_OF_SERVANTS
#define MAX_NUM_OF_SERVANTS     12
#endif

#ifndef MAX_PROTO_SIZE
#define MAX_PROTO_SIZE          8
#endif

#ifndef MAX_OPERATION_NAME_SIZE
#define MAX_OPERATION_NAME_SIZE 16
#endif

#endif /* _ICE_PLATFORM_ARDUINO_H_ */

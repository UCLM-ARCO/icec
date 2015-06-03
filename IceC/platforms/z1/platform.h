/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_PLATFORM_Z1_H_
#define _ICE_PLATFORM_Z1_H_

#define ICE_LITTLE_ENDIAN
#define ICEC_FUNC_ATTR

#include <stdlib.h>

#include <IceC/platforms/z1/types.h>
#include <IceC/platforms/z1/debug.h>
#include <IceC/platforms/z1/strings.h>
#include <IceC/platforms/z1/errors.h>

#define MAX_MESSAGE_SIZE        128
#define MAX_IDENTITY_SIZE       16
#define MAX_NESTED_ENCAPS       3
#define MAX_NUM_OF_ADAPTERS     1
#define MAX_NUM_OF_SERVANTS     1
#define MAX_PROTO_SIZE          16
#define MAX_OPERATION_NAME_SIZE 16

#endif /* _ICE_PLATFORM_Z1_H_ */

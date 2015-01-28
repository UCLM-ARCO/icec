/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_PLATFORM_X86_H_
#define _ICE_PLATFORM_X86_H_

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <assert.h>
#include <errno.h>
#include <error.h>

#include <IceC/platforms/x86/types.h>
#include <IceC/platforms/x86/debug.h>

#define ICE_LITTLE_ENDIAN

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE        128
#endif

#define MAX_IDENTITY_SIZE       16
#define MAX_NESTED_ENCAPS       5
#define MAX_NUM_OF_ADAPTERS     5
#define MAX_NUM_OF_SERVANTS     5
#define MAX_PROTO_SIZE          16
#define MAX_OPERATION_NAME_SIZE 16

#endif /* _ICE_PLATFORM_X86_H_ */

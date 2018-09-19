/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_PLATFORM_X86_H_
#define _ICE_PLATFORM_X86_H_

#define _GNU_SOURCE

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
#define ICEC_FUNC_ATTR

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE        512
#endif

#ifndef MAX_IDENTITY_SIZE
#define MAX_IDENTITY_SIZE       64
#endif

#ifndef MAX_ADAPTER_ID_SIZE
#define MAX_ADAPTER_ID_SIZE     64
#endif

#ifndef MAX_NESTED_ENCAPS
#define MAX_NESTED_ENCAPS       5
#endif

#ifndef MAX_NUM_OF_ADAPTERS
#define MAX_NUM_OF_ADAPTERS     5
#endif

#ifndef MAX_NUM_OF_SERVANTS
#define MAX_NUM_OF_SERVANTS     5
#endif

#ifndef MAX_PROTO_SIZE
#define MAX_PROTO_SIZE          8
#endif

#ifndef MAX_OPERATION_NAME_SIZE
#define MAX_OPERATION_NAME_SIZE 32
#endif

#ifndef MAX_EXCEPTION_SIZE
#define MAX_EXCEPTION_SIZE      32
#endif

#endif /* _ICE_PLATFORM_X86_H_ */

/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_TYPES_H_
#define _ICEC_TYPES_H_

#include <stdint.h>

#ifdef ARDUINO_ARCH_AVR
typedef uint16_t ssize_t;
#endif

#if !defined(__cplusplus) && !defined(BOOL_TYPE)
#undef false
#undef true

#define BOOL_TYPE
#define bool char
#define false 0
#define true 1

#endif

#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef uint8_t byte;
#endif

#endif  /* _ICEC_TYPES_H_ */

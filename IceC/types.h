/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_TYPES_H_
#define _ICEC_TYPES_H_

#include <stdint.h>

#ifndef __cplusplus
typedef enum {false=0, true=1} bool;
#endif

#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef uint8_t byte;
#endif

#endif  /* _ICEC_TYPES_H_ */

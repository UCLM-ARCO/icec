/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_ICEUTIL_H_
#define _ICEC_ICEUTIL_H_

#include <IceC/IceC.h>

#ifdef __cplusplus
extern "C" {
#endif

void IceUtil_getHost(const char* endpoint, char* result);
void IceUtil_getPort(const char* endpoint, uint16_t* result);
void IceUtil_getStringParam(const char* input, char* result, char flag);

#ifdef __cplusplus
}
#endif

#endif /* _ICEC_ICEUTIL_H_ */

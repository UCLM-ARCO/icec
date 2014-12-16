/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_ICEUTIL_H_
#define _ICEC_ICEUTIL_H_

#include <IceC/IceC.h>

void IceUtil_getHost(const char* endpoint, char* result);
void IceUtil_getPort(const char* endpoint, uint16_t* result);

#endif /* _ICEC_ICEUTIL_H_ */

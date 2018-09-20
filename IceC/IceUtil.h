/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_ICEUTIL_H_
#define _ICEC_ICEUTIL_H_

#include <IceC/IceC.h>

#ifdef __cplusplus
extern "C" {
#endif

void IceUtil_getIdentity(const char* s, char* result);
void IceUtil_getProtocolType(const char* s, Ice_EndpointType* result);
void IceUtil_getMode(const char* s, Ice_ReferenceMode* mode);
void IceUtil_getAdapterId(const char* s, char* result);

void IceUtil_getHost(const char* endpoint, char* result);
void IceUtil_getPort(const char* endpoint, uint16_t* result);
void IceUtil_getStringParam(const char* input, char* result, char flag);
void IceUtil_getByteParam(const char* input, byte* result, char flag);

void IceUtil_delay(uint16_t ms);
void IceUtil_IdentityToString(IceStd_IdentityPtr oid, char* dst);

#ifdef __cplusplus
}
#endif

#endif /* _ICEC_ICEUTIL_H_ */

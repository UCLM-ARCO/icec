/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_STRINGS_H_
#define _ICEC_STRINGS_H_

#include <string.h>

#ifndef ARDUINO_ARCH_SAMD

static const char*
index(const char* s, int c) {
    while(*s != 0) {
	if (*s == c)
	    return s;
	s++;
    }
    return NULL;
}

#endif /* ARDUINO_ARCH_SAMD */

#endif /* _ICEC_STRINGS_H_ */

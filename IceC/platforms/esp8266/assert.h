/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

extern void prints(const char*);
extern void printi(const int);

#if defined(ARDUINO_ARCH_ESP32)
#include <assert.h>
#else
#define assert(token) __assert(token, #token)

static void
__assert(bool status, const char* message) {
    if (status)
	return;

    prints("AssertionError: ");
    prints(message);
    prints("\n");

    abort();
}
#endif

#endif /* _ICE_ASSERT_H_ */

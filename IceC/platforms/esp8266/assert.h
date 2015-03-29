/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

#define assert(token) __assert(token, #token)

static void
__assert(bool status, const char* message) {
    if (status)
	return;

    os_printf("AssertionError: %s\n", message);
    abort();
}

#endif /* _ICE_ASSERT_H_ */

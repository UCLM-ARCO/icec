/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

#include "xil_printf.h"

#define assert(token) __assert(token, #token)

static void
__assert(bool status, const char* message) {
    if (status)
	return;

    xil_printf("AssertionError: ");
    xil_printf(message);
    xil_printf("\n");

    do {} while (1);
}

#endif /* _ICE_ASSERT_H_ */

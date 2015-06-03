/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ERRORS_H_
#define _ICE_ERRORS_H_

#include <contiki.h>
#include <dev/leds.h>

#ifndef DEBUG
#  define error_at_line(...)
#  define fixme(...)
#  define assert(token) __assert(token)

static void
__assert(bool status) {
    if (status)
	return;

    while(true) {
	leds_on(LEDS_RED);
	clock_wait(CLOCK_SECOND/20);
	leds_off(LEDS_RED);
	clock_wait(CLOCK_SECOND - (CLOCK_SECOND/20));
    }
}

#else

#  undef assert
#  define fixme(msg) printf(msg);
#  define error_at_line(...)
#  define assert(token) __assert(token, #token)

static void
__assert(bool status, const char* message) {
    if (status)
	return;

    printf("AssertionError: %s\n", message);

    while(true) {
	leds_on(LEDS_RED);
	clock_wait(CLOCK_SECOND/20);
	leds_off(LEDS_RED);
	clock_wait(CLOCK_SECOND - (CLOCK_SECOND/20));
    }
}

#endif  /* DEBUG */
#endif  /* _ICE_ERRORS_H_ */

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
#  define error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    printf("ERROR, " FILE ":" #LINE ": " MESSAGE "\n");

#  define assert(token) __assert(token, #token, __FILE__, __LINE__)

static void
__assert(bool status, const char* message, char* file, uint16_t line) {
    if (status)
	return;

    printf("AssertionError: %s:%d: %s\n", file, line, message);

    while(true) {
	leds_on(LEDS_RED);
	clock_wait(CLOCK_SECOND/20);
	leds_off(LEDS_RED);
	clock_wait(CLOCK_SECOND - (CLOCK_SECOND/20));
    }
}

#endif  /* DEBUG */
#endif  /* _ICE_ERRORS_H_ */

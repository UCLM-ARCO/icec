/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

#ifndef DEBUG
#  define error_at_line(...)
#  define assert(token) __assert(token)
#else
#  define error_at_line(s, e, f, l, m) __DBG_error_at_line(s, e, f, l, m)
#  define assert(token) __DBG_assert(token, #token)
#endif

extern void prints(const char*);

#ifdef DEBUG_LED
extern void led_config();
extern void led_blink(int);
#endif

#define __DBG_error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    prints("Error, " FILE ":" #LINE ": " MESSAGE);

static void
__assert(bool status) {
    if (status)
	return;

#ifdef DEBUG_LED
    led_config();
    while(true) {
	led_blink(1000);
    }
#else
    while(true);
#endif

}

static void
__DBG_assert(bool status, const char* message) {
    if (status)
	return;

    prints("E: ");
    prints(message);
    prints("\n");

    /* assertion failed, stop here */
#ifdef DEBUG_LED
    led_config();
    while(true) {
	led_blink(1000);
    }
#else
    while(true);
#endif
}

#endif /* _ICE_ASSERT_H_ */

/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

#define error_at_line(...)

extern void prints(const char*);
extern void led_config();
extern void led_blink(int);

#ifdef DEBUG
#define assert(token) __DBG_assert(token, #token)
#else

static void
assert(bool status) {
    if (status)
	return;

    while(true);
}
#endif

static void
__DBG_assert(bool status, const char* message) {
    if (status)
	return;

    prints("E: ");
    prints(message);
    prints("\n");

    /* assertion failed, stop here */
    led_config();
    while(true) {
	led_blink(1000);
    }
}

#define DBG_assert(token) __DBG_assert(token, #token)
#define DBG_error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    prints("Error, " FILE ":" #LINE ": " MESSAGE);

#endif /* _ICE_ASSERT_H_ */

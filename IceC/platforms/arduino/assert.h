/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

#define assert(...)
#define error_at_line(...)

extern void prints(const char*);

static void
__DBG_assert(bool status, const char* message) {
    if (status)
	return;

    prints("assertion failed: ");
    prints(message);
    prints("\n");

    /* assertion failed, stop here */
    while(true);
}

#define DBG_assert(token) __DBG_assert(token, #token)
#define DBG_error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    prints("Error, " FILE ":" #LINE ": " MESSAGE);

#endif /* _ICE_ASSERT_H_ */

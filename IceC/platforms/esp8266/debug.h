/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

// See flags.h
#ifndef DEBUG
#  define trace()
#  define fixme(...)
#  define error_at_line(...) while(1);   // force a reset
#else

extern void prints(const char*);
extern void printi(const int);
extern int printh(const int);

static void
hexdump(byte* src, int size) {
    int i;

    for(i = 0; i < size; i++) {
	if (src[i] < 16)
	    prints("0");

	printh(src[i]);
	prints(" ");
	if ((i + 1) % 8 == 0)
	    prints("\n");
    }
}

#define trace()					\
    prints(__FILE__ ":");			\
    printi(__LINE__);				\
    prints(" ");				\
    prints(__func__);				\
    prints("\n");

#define fixme(MSG)			        \
    prints("FIXME: " MSG "(" __FILE__ ":");	\
    printi(__LINE__);				\
    prints(" ");				\
    prints(__func__);				\
    prints(")\n");

#define error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    prints("Error, " FILE ":" #LINE ": " MESSAGE); \
    while(1);   // force a reset

#endif  /* DEBUG */
#endif  /* _ICEC_DEBUG_H_ */

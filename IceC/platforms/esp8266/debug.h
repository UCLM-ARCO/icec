/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#ifndef DEBUG
#  define trace()
#  define fixme(...)
#  define error_at_line(...)
#else

extern void prints(const char*);
extern void printi(const int);

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
    prints("Error, " FILE ":" #LINE ": " MESSAGE);

#endif /* DEBUG */
#endif  /* _ICEC_DEBUG_H_ */

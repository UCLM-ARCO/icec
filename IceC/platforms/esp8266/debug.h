/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#include "flags.h"

// See flags.h
#ifndef DEBUG
#  define trace()
#  define fixme(...)
#  define error_at_line(...) while(1);   // force a reset
#  define hexdump(...)
#else

extern void prints(const char*);
extern void printi(const int);
extern int printh(const int);

static void
hexdump (void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                printf ("  %s\n", buff);
            printf ("  %04x ", i);
        }

        printf (" %02x", pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    printf ("  %s\n", buff);
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

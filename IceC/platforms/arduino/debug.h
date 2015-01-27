/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#define trace()
#define fixme(...)

extern void prints(const char*);
extern void printi(const int);

#define DBG_trace()				\
    prints(__FILE__);				\
    prints(":");				\
    printi(__LINE__);				\
    prints(" ");				\
    prints(__func__);				\
    prints("\n");

static int
get_free_ram_size() {
    extern int __heap_start, *__brkval;
    int v;
    int a = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return a;
}

#endif  /* _ICEC_DEBUG_H_ */

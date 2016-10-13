/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_ARDUINO_DEBUG_H_
#define _ICEC_ARDUINO_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <IceC/platforms/arduino/flags.h>
#include <IceC/platforms/arduino/types.h>

// used regardless of the state of DEBUG flag
void print_dbg(const char* msg);
void printi_dbg(const int i);

#ifndef LOGGER
    #define print_log(...)
    #define printi_log(...)
#else
    void print_log(const char* s);
    void printi_log(const int i);
#endif

#ifndef DEBUG
    #define trace()
    #define fixme(...)
    #define prints(...)
#else

    void trace();
    void fixme(const char*);
    void prints(const char*);
// extern void printsln(const char*);
// extern void printi(const int);
// extern void printhex(const int);
// extern void printb(const uint8_t* msg, uint8_t size);
// extern void printp(const char*, const int);
// extern void hexdump(const void* addr, int len);

// #define DBG_trace()				\
//     prints(__FILE__);				\
//     prints(":");				\
//     printi(__LINE__);				\
//     prints(" ");				\
//     prints(__func__);				\
//     prints("\n");

// /* got somewhere on SO or Arduino Playground */
// static int
// get_free_ram_size() {
//     extern int __heap_start, *__brkval;
//     int v;
//     int a = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
//     return a;
// }
#endif

#ifndef DEBUG_LED
    #define led_config()
    #define led_blink(...)
#else

    #ifndef DEBUG_LED_PIN
    #define DEBUG_LED_PIN 13
    #endif

    void led_config();
    void led_blink(int wait);

#endif

#ifdef __cplusplus
}
#endif

#endif  /* _ICEC_ARDUINO_DEBUG_H_ */

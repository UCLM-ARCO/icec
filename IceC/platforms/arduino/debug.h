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
void printh_dbg(const int i);
void hexdump(const void* addr, int len);

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

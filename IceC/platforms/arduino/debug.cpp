// -*- mode: c++; coding: utf-8; truncate-lines: true -*-

#include "debug.h"
#include <Arduino.h>

#ifdef ARDUINO_ARCH_SAMD
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

// used regardless of the state of DEBUG flag
void print_dbg(const char* msg) {
    SERIAL.print(msg);
    SERIAL.flush();
}

void printi_dbg(const int i) {
    SERIAL.print(i, DEC);
    SERIAL.flush();
}

void printh_dbg(const int i) {
    if (i<16)
        SERIAL.print("0");
    SERIAL.print(i, HEX);
    SERIAL.flush();
}

void hexdump(const void* addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char* pc = (unsigned char*)addr;
    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0) {
                print_dbg("  ");
                print_dbg((const char*)buff);
                print_dbg("\n");
            }
            print_dbg("  ");
            printh_dbg(i);
            print_dbg(" ");
        }
        print_dbg(" ");
        printh_dbg(pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        print_dbg("   ");
        i++;
    }
    print_dbg("  ");
    print_dbg((const char*)buff);
    print_dbg("\n");
}

#ifdef LOGGER  // -------------------------------------------------------
#include <SoftwareSerial.h>

static SoftwareSerial logger(10, 11); // Rx, Tx

void
logger_init() {
    static bool logger_initialized = false;
    if (not logger_initialized)
        logger.begin(115200);
}

void
print_log(const char* s) {
    logger_init();
    logger.write(s);
    logger.flush();
}

void
printi_log(const int i) {
    logger_init();
    logger.print(i, 16);
    logger.flush();
}
#endif

#ifdef DEBUG  // -------------------------------------------------------

void fixme(const char* msg) {
    // NOTE: not implemented to avoid consuming too much space
}

void
prints(const char* msg) {
    SERIAL.print(msg);
    SERIAL.flush();
}

void
printi(const int v) {
    SERIAL.print(v);
    SERIAL.flush();
}
#endif

#ifdef DEBUG_LED  // ---------------------------------------------------
void
led_config() {
    pinMode(DEBUG_LED_PIN, OUTPUT);
}

void
led_blink(int wait) {
    for (byte i=0; i<4; i++) {
        digitalWrite(DEBUG_LED_PIN, (i+1)%2);
        delay(50);
    }

    delay(wait);
}
#endif

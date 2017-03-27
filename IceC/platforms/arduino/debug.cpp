// -*- mode: c++; coding: utf-8; truncate-lines: true -*-

#include "debug.h"
#include <Arduino.h>

// used regardless of the state of DEBUG flag
void print_dbg(const char* msg) {
    Serial.print(msg);
    Serial.flush();
}

void printi_dbg(const int i) {
    Serial.print(i, DEC);
    Serial.flush();
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
void trace() {
    // NOTE: not implemented to avoid consuming too much space
}

void fixme(const char* msg) {
    // NOTE: not implemented to avoid consuming too much space
}

void
prints(const char* msg) {
    Serial.print(msg);
    Serial.flush();
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

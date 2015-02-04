/* -*- mode: c++; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_DEBUG_HPP_
#define _ICE_DEBUG_HPP_

#include <HardwareSerial.h>

#ifndef DEBUG_LED
#define DEBUG_LED 13
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void
    debug_init() {
	Serial.begin(115200);
	while(!Serial);
    }

    void
    prints(const char* msg) {
	Serial.print(msg);
	Serial.flush();
    }

    void
    printb(const uint8_t* msg, uint8_t size) {
	Serial.write(msg, size);
	Serial.flush();
    }

    void
    printi(const int v) {
	Serial.print(v);
	Serial.flush();
    }

    void
    led_config() {
	pinMode(DEBUG_LED, OUTPUT);
    }

    void
    led_blink(int wait) {
	digitalWrite(DEBUG_LED, 1);
	delay(50);
	digitalWrite(DEBUG_LED, 0);
	delay(50);
	digitalWrite(DEBUG_LED, 1);
	delay(50);
	digitalWrite(DEBUG_LED, 0);

	delay(wait);
    }

#ifdef __cplusplus
}
#endif

#endif /* _ICE_DEBUG_HPP_ */

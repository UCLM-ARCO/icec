/* -*- mode: c++; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_DEBUG_HPP_
#define _ICE_DEBUG_HPP_

#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#ifdef LOGGER
static SoftwareSerial logger(5, 6); // Rx, Tx

void
print_log(const char* s) {
    static bool logger_initialized = false;
    if (not logger_initialized)
	logger.begin(9600);

    logger.write(s);
    logger.flush();
}
#endif

#ifndef DEBUG_LED
#define DEBUG_LED 13
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void
    debug_init() {
	led_config();
	Serial.begin(115200);
	while(!Serial);
    }

    void
    prints(const char* msg) {
	Serial.print(msg);
	Serial.flush();
    }

    void
    printsln(const char* msg) {
	Serial.println(msg);
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
    printp(const char* msg, const int i) {
	prints(msg);
	printi(i);
	prints("\n");
    }

    char*
    tohex(uint8_t d) {
	char alpha[] = {'0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	uint8_t l = d / 16;
	uint8_t r = d % 16;
	static char result[3];
	result[0] = alpha[l];
	result[1] = alpha[r];
	result[2] = 0;
	return result;
    }

    void
    printhex(const int v) {
	Serial.print(tohex(v));
	Serial.flush();
    }

    void
    hexdump(const void* addr, int len) {
	int i;
	unsigned char buff[17];
	unsigned char* pc = (unsigned char*)addr;

	for (i = 0; i < len; i++) {
	    if ((i % 16) == 0) {
		if (i != 0) {
		    prints("  ");
		    prints((const char*)buff);
		    prints("\n");
		}

		prints("  ");
		printhex(i);
		prints(" ");
	    }

	    prints(" ");
	    printhex(pc[i]);

	    if ((pc[i] < 0x20) || (pc[i] > 0x7e))
		buff[i % 16] = '.';
	    else
		buff[i % 16] = pc[i];
	    buff[(i % 16) + 1] = '\0';
	}

	while ((i % 16) != 0) {
	    prints("   ");
	    i++;
	}

	prints("  ");
	prints((const char*)buff);
	prints("\n");
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

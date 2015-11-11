/* -*- mode: c++; coding: utf-8; truncate-lines: true -*- */

#ifdef ARDUINO_ARCH_AVR

#include <EEPROM.h>

#include "storage.h"

void
IceC_Storage_begin() {
    // Nothing to do here
}

void
IceC_Storage_clear() {
    for (uint16_t i=0; i<EEPROM.length(); i++)
	EEPROM.update(i, 0);
}

void
IceC_Storage_get(uint16_t addr, char* dst, uint16_t size) {
    if (addr + size > EEPROM.length())
    	return;

    for (uint16_t i=0; i<size; i++)
	*(dst+i) = (char)EEPROM.read(addr+i);
}

void
IceC_Storage_put(uint16_t addr, const char* src, uint16_t size) {
    if (addr + size > EEPROM.length())
	return;

    for (uint16_t i=0; i<size; i++)
	EEPROM.update(addr+i, *(src+i));
}

#endif /* ARDUINO_ARCH_AVR */

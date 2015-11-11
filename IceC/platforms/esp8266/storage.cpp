/* -*- mode: c++; coding: utf-8; truncate-lines: true -*- */

#include <EEPROM.h>

#include "storage.h"

void
IceC_Storage_begin() {
    EEPROM.begin(EEPROM_SIZE);
}

void
IceC_Storage_clear() {
    for (uint16_t i=0; i<EEPROM_SIZE; i++)
	EEPROM.write(i, 0);
    EEPROM.commit();
}

void
IceC_Storage_get(uint16_t addr, char* dst, uint16_t size) {
    if (addr + size > EEPROM_SIZE)
    	return;

    for (uint16_t i=0; i<size; i++)
	*(dst+i) = (char)EEPROM.read(addr+i);
}

void
IceC_Storage_put(uint16_t addr, const char* src, uint16_t size) {
    if (addr + size > EEPROM_SIZE)
    	return;

    for (uint16_t i=0; i<size; i++)
	EEPROM.write(addr+i, *(src+i));
    EEPROM.commit();
}

/* -*- mode: c++; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICEC_STORAGE_H_
#define _ICEC_STORAGE_H_

#include <stdint.h>

#define EEPROM_SIZE 512

#ifdef __cplusplus
extern "C" {
#endif

void IceC_Storage_begin();
void IceC_Storage_clear();
void IceC_Storage_get(uint16_t addr, char* dst, uint16_t size);
void IceC_Storage_put(uint16_t addr, const char* src, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif // _ICEC_STORAGE_H_

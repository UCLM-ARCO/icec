/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

#include "debug.h"

#ifdef ARDUINO_ARCH_SAMD
#include <assert.h>
#endif

#ifndef DEBUG
  /* NOTE: not implemented to avoid consuming too much space */
  #define error_at_line(...)

  #ifndef ARDUINO_ARCH_SAMD
  static void assert(bool status) {
      if (status)
	  return;

      #ifdef DEBUG_LED
      {
	  led_config();
	  byte c=3;
	  while(c) {
	      led_blink(1000);
	      c--;
	  }

	  /* soft reset */
	  asm volatile ("jmp 0");
      }

      #else
      {
	  /* soft reset */
	  asm volatile ("jmp 0");
      }
      #endif
  }
  #endif /* ARDUINO_ARCH_SAMD */

#else
  #define error_at_line(s, e, f, l, m) __DBG_error_at_line(s, e, f, l, m)
  #define __DBG_error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
      prints("Error, " FILE ":" #LINE ": " MESSAGE);

  #ifndef ARDUINO_ARCH_SAMD
  #define assert(token) __DBG_assert(token, #token)

  static void
  __DBG_assert(bool status, const char* message) {
      if (status)
	  return;

      /* prints("E: "); */
      /* prints(message); */
      /* prints("\n"); */

      /* assertion failed, stop here */
      #ifdef DEBUG_LED
      {
	  led_config();
	  while(true) {
	      led_blink(1000);
	  }
      }

      #else
      {
	  while(true);
      }
      #endif
  }
  #endif /* ARDUINO_ARCH_SAMD */

#endif

#endif /* _ICE_ASSERT_H_ */

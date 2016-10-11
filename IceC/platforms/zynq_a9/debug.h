/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#define DEBUG
#ifndef DEBUG
#  define trace()
#  define fixme(...)
#  define error_at_line(...) while(1);   // force a reset
#else

#define trace()	xil_printf(" -> %s:%d %s\n", __FILE__, __LINE__, __func__);
#define fixme(MSG)	xil_printf("FIXME: %s (%s:%d %s)\n", \
			  	  	  	  	  MSG, __FILE__, __LINE__, __func__)

#define error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    xil_printf("Error, %s:%i : %s", FILE, #LINE, MESSAGE); \
    while(1);   // force a reset

#endif  /* DEBUG */
#endif  /* _ICEC_DEBUG_H_ */

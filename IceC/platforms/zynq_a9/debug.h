/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#define prints xil_printf

#define DEBUG
#ifndef DEBUG
#  define trace()
#  define fixme(...)
#  define hexdump(...)
#  define error_at_line(...) while(1);   // force a reset
#else

#define trace()	xil_printf(" -> %s:%d %s\n", __FILE__, __LINE__, __func__);
#define fixme(MSG)	xil_printf("FIXME: %s (%s:%d %s)\n", \
			  	  	  	  	  MSG, __FILE__, __LINE__, __func__)
#define error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    xil_printf("Error, %s:%i : %s", FILE, #LINE, MESSAGE); \
    while(1);   // force a reset

/* from: http://stackoverflow.com/a/7776146 */
static void
hexdump (void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                xil_printf ("  %s\n", buff);
            xil_printf ("  %04x ", i);
        }

        xil_printf (" %02x", pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        xil_printf ("   ");
        i++;
    }

    xil_printf ("  %s\n", buff);
}

#endif  /* DEBUG */
#endif  /* _ICEC_DEBUG_H_ */

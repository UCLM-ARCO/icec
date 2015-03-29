/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#ifndef DEBUG
#  define trace()
#  define fixme(...)
#  define error_at_line(...)
#else

#define trace()							\
    os_printf("%s:%d %s\n", __FILE__, __LINE__, __func__);

#define fixme(MSG)							\
    os_printf("FIXME: %s (%s:%d %s)\n", MSG, __FILE__, __LINE__, __func__);

#define error_at_line(STATUS, ERRNUM, FILE, LINE, MESSAGE)	\
    os_printf("Error, " FILE ":" #LINE ": " MESSAGE);

#endif /* DEBUG */
#endif  /* _ICEC_DEBUG_H_ */

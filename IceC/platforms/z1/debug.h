/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#ifndef DEBUG
#  define trace()

#else

#  include <stdio.h>
#  include <assert.h>
#  define trace() printf(" -> %s:%d %s\n", __FILE__, __LINE__, __func__)

#endif  /* DEBUG */

#endif  /* _ICEC_DEBUG_H_ */

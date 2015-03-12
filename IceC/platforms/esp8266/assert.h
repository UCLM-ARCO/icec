/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ASSERT_H_
#define _ICE_ASSERT_H_

static void
assert(bool status) {
    if (status)
        return;
  
    while(true);
}

#endif /* _ICE_ASSERT_H_ */

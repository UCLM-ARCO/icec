/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICEC_XBEE_AT_H_
#define _ICEC_XBEE_AT_H_

#include "xbee-at.h"

#define Ice_EndpointTypeXBee 10

static void
Ice_XBeeConnection_setup(Ice_ConnectionPtr this) {
    trace();
    Ptr_check(this);
    this->fd = xbee_at_init(serialport);
}


#endif // _ICEC_XBEE_AT_H_

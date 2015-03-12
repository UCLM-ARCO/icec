/* -*- mode: c; coding: utf-8 -*- */

#include <stdio.h>
#include <IceC/IceC.h>
// #include "UDPEndpoint.h"

#include "hello.h"

void
user_init() {
    Ice_Communicator ic;
    Ice_ObjectPrx writer;
    char* str_proxy = "";

    /* create communicator */
    Ice_initialize(&ic);
    // UDPEndpoint_init(&ic);

    /* create proxy to server */
    Ice_Communicator_stringToProxy(&ic, str_proxy, &writer);

    /* invoke it */
    Example_Writer_write(&writer, 1551);
}

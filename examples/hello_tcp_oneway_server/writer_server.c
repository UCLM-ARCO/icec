/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/TCPEndpoint.h>
#include "hello.h"

void
Example_WriterI_write(Example_WriterPtr this, int code) {
    printf(" - 'write' request, got: %d\n", code);
    fflush(NULL);
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Writer servant;

    /* create communicator */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* create object adapter */
    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "tcp -h 127.0.0.1 -p 7894", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_Writer_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "WriterServer");

    /* wait for events (event loop) */
    printf("Proxy ready: 'WriterServer -e 1.0 -o:tcp -h 127.0.0.1 -p 7894'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

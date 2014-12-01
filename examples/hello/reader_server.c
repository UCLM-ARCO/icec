/* -*- mode: c; coding: utf-8 -*- */

#include "hello.h"

int
Example_ReaderI_read(Example_ReaderPtr this) {
    static int retval = 1441;
    printf(" - 'read' request, return: %d\n", retval);
    fflush(NULL);

    return retval++;
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Reader servant;

    /* create communicator */
    Ice_initialize(&ic);

    /* create object adapter */
    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "tcp -h 127.0.0.1 -p 7891", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_Reader_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "ReaderServer");

    /* wait for events (event loop) */
    printf("Proxy ready: 'ReaderServer -e 1.0 -t:tcp -h 127.0.0.1 -p 7891'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);

    return 0;
}

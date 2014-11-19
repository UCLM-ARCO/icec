/* -*- mode: c; coding: utf-8 -*- */

#include "slice.h"

char*
Example_ReaderI_read(Example_ReaderPtr this) {
    static char* message = "the message from server";
    return message;
}

int
main(int argc, char* argv[]) {

    // create communicator
    Ice_Communicator ic;
    Ice_initialize(&ic);

    // create object adapter
    Ice_ObjectAdapter adapter;
    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "tcp -h 127.0.0.1 -p 7891", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    // register servant
    Example_Reader servant;
    Example_Reader_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "ReaderServer");

    // wait for events (event loop)
    Ice_Communicator_waitForShutdown(&ic);

    return 0;
}

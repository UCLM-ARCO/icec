/* -*- mode: c; coding: utf-8 -*- */

#include "slice.h"
#include "myplugin.h"

void Example_ConfigI_set(Example_ConfigPtr this, int code) {
    printf("set called with code: %d\n", code);
    fflush(NULL);
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Config servant;

    /* create communicator */
    Ice_initialize(&ic);

    /* init plugin */
    MyPlugin_init(&ic);

    /* create object adapter */
    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "file -p /tmp/myendp", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_Config_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "ConfigServer");

    /* wait for events (event loop) */
    printf("Proxy ready: 'ConfigServer -e 1.0 -o:file -p /tmp/myendp'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);

    return 0;
}

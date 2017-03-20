/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/platforms/x86/UDPEndpoint.h>
#include "slice.h"

#define PORT "7900"

void
Example_ServiceI_ping(Example_ServicePtr this) {
    printf(" - 'ping' request");
    fflush(NULL);
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Service servant;

    /* create communicator */
    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    /* create object adapter */
    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "udp -h 127.0.0.1 -p " PORT, &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_Service_init(&servant);
    Ice_ObjectAdapter_addDefaultServant(&adapter, (Ice_ObjectPtr)&servant);

    /* wait for events (event loop) */
    printf("Proxy ready: 'XXXX -e 1.0 -d:udp -h 127.0.0.1 -p " PORT "'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

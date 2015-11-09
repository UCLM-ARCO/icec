/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/UDPEndpoint.h>

#include <stdio.h>

#include "slice.h"

void
Base_Example_Iface1I_sayHello(Base_Example_Iface1Ptr self) {
    printf("hello\n");
    fflush(NULL);
}

void
Extended_Iface2I_sayBye(Extended_Iface2Ptr self) {
    printf("bye\n");
    fflush(NULL);
}

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Extended_Iface2 servant;

    (void) argc;
    (void) argv;

    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "udp -h 127.0.0.1 -p 7892", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    Extended_Iface2_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "Server");

    printf("Proxy ready: 'Server -e 1.0 -d:udp -h 127.0.0.1 -p 7892'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/UDPEndpoint.h>

#include "example.h"

void
Example_HelloI_sayHelloFrom(Example_HelloPtr self, Ice_String name, int times) {
    char str_name[64];

    strcpy(str_name, name.value);
    str_name[name.size] = 0;

    printf(" - 'sayHelloFrom' request, from: '%s', times: %d\n", str_name, times);
    fflush(NULL);
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Hello servant;
    char* endp;

    /* create communicator */
    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    /* create object adapter */
    endp = "udp -h 127.0.0.1 -p 7896";
    Ice_Communicator_createObjectAdapterWithEndpoints(&ic, "Adapter", endp, &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_Hello_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "HelloServer");

    /* wait for events (event loop) */
    printf("Proxy ready: 'HelloServer -e 1.0 -d:%s'\n", endp);
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

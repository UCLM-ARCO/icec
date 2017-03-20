/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/platforms/x86/TCPEndpoint.h>

#define PORT "7901"
#define OID  "DynServer"

void
MyExample_ice_invoke(Ice_BlobjectPtr this, Ice_InputStreamPtr is, Ice_Current current,
		     Ice_OutputStreamPtr os) {

    printf(" - 'ice_invoke' called\n");
    printf("   - identity: '%s'\n", current.id);
    printf("   - operation: '%s'\n", current.operation);
    fflush(NULL);
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Ice_Blobject servant;

    /* create communicator */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* create object adapter */
    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "tcp -h 127.0.0.1 -p " PORT, &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Ice_Blobject_init(&servant, &MyExample_ice_invoke);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, OID);

    /* wait for events (event loop) */
    printf("Proxy ready: '" OID " -e 1.0 -o:tcp -h 127.0.0.1 -p " PORT "'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

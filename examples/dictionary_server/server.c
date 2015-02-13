/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/UDPEndpoint.h>

#include "example.h"

void
Example_PropertyServiceI_update(Example_PropertyServicePtr self,
				Example_StringDict props) {
    byte i;
    char key[64];
    char value[64];

    printf(" - 'update' request, {");

    for(i=0; i<props.size; i++) {
	Example_StringDict_PairPtr item = &(props.items[i]);

	strncpy(key, item->key.value, item->key.size);
	key[item->key.size] = 0;
	printf("'%s': ", key);

	strncpy(value, item->value.value, item->value.size);
	value[item->value.size] = 0;
	printf("'%s'", value);

	if (i + 1 < props.size)
	    printf(", ");
    }

    printf("}\n");
    fflush(NULL);
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_PropertyService servant;
    char* endp;

    /* create communicator */
    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    /* create object adapter */
    endp = "udp -h 127.0.0.1 -p 7897";
    Ice_Communicator_createObjectAdapterWithEndpoints(&ic, "Adapter", endp, &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_PropertyService_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "PropertyServer");

    /* wait for events (event loop) */
    printf("Proxy ready: 'PropertyServer -e 1.0 -d:%s'\n", endp);
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

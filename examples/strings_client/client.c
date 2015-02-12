/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/UDPEndpoint.h>

#include "example.h"

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectPrx server;
    Ice_String name;

    if (argc != 2) {
	printf("USAGE: %s <proxy>\n", argv[0]);
	return 1;
    };

    /* create communicator */
    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    /* create proxy */
    Ice_Communicator_stringToProxy(&ic, argv[1], &server);

    /* call remote method */
    name.value = "Catherine Foo";
    name.size = strlen(name.value);

    Example_Hello_sayHelloFrom(&server, name, 25);
    return 0;
}

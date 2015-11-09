/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/UDPEndpoint.h>

#include <stdio.h>

#include "slice.h"

int
main(int argc, char** argv) {
    Ice_Communicator ic;

    Ice_ObjectPrx server;

    if (argc < 2) {
	printf("USAGE: %s <proxy>\n", argv[0]);
	return -1;
    }

    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    Ice_Communicator_stringToProxy(&ic, argv[1], &server);
    Base_Example_Iface1_sayHello(&server);
    Extended_Iface2_sayBye(&server);
    return 0;
}

/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
// #include "UDPEndpoint.h"

#include "hello.h"

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectPrx writer;

    if (argc < 2)
	return printf("USAGE: %s <proxy>\n", argv[0]);

    /* create communicator */
    Ice_initialize(&ic);
    // UDPEndpoint_init(&ic);

    /* create proxy to server */
    Ice_Communicator_stringToProxy(&ic, argv[1], &writer);

    /* invoke it */
    Example_Writer_write(&writer, 1551);

    return 0;
}

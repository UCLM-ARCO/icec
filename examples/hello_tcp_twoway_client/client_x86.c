/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/platforms/x86/TCPEndpoint.h>
#include "hello.h"

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectPrx reader;
    int result;

    if (argc < 2)
	return printf("USAGE: %s <proxy>\n", argv[0]);

    /* create communicator */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* create proxy to server */
    Ice_Communicator_stringToProxy(&ic, argv[1], &reader);

    /* read invocation*/
    result = Example_Reader_read(&reader);
    printf("returned value is: %d\n", result);

    /* IMPORTANT! close each proxy connection */
    Ice_ObjectPrx_closeConnection(&reader);

    return 0;
}

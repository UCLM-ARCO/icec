/* -*- mode: c; coding: utf-8 -*- */

#include "slice.h"

int
main(int argc, char* argv[]) {
    if (argc < 2)
	return printf("USAGE: %s <proxy>\n", argv[0]);

    // create communicator
    Ice_Communicator ic;
    Ice_initialize(&ic);

    // create proxy to server
    Ice_ObjectPrx writer;
    Ice_Communicator_stringToProxy(&ic, argv[1], &writer);

    // invoke it
    Example_Writer_write(&writer, 1551);

    return 0;
}

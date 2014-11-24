/* -*- mode: c; coding: utf-8 -*- */

#include "slice.h"
#include "myplugin.h"

int
main(int argc, char* argv[]) {
    if (argc < 2) {
	printf("USAGE: %s <proxy>\n", argv[0]);
	return -1;
    }

    // create communicator
    Ice_Communicator ic;
    Ice_initialize(&ic);

    // init plugin
    MyPlugin_init(&ic);

    // create proxy to server
    Ice_ObjectPrx config;
    Ice_Communicator_stringToProxy(&ic, argv[1], &config);

    // invoke it
    Example_Config_set(&config, 1551);
    return 0;
}

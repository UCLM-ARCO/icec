/* -*- mode: c; coding: utf-8 -*- */

#include <stdio.h>
#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

#include "floats.h"


int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectPrx thermostat;

    if (argc < 2) {
	printf("USAGE: %s <proxy>\n", argv[0]);
	return -1;
    }

    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    Ice_Communicator_stringToProxy(&ic, argv[1], &thermostat);

    float temp = 25.6;
    Example_Thermostat_setTemperature(&thermostat, temp);

    return 0;
}

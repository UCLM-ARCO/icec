/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>
#include <IceC/platforms/x86/debug.h>

#include <stdio.h>
#include <math.h>

#include "floats.h"

void
Example_ThermostatI_setTemperature(Example_ThermostatPtr this, Ice_Float temp) {
    printf("setTemperature to %f C\n", temp);
    fflush(NULL);
}

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Thermostat servant;

    (void) argc;
    (void) argv;

    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "tcp -h 127.0.0.1 -p 7892", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    Example_Thermostat_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "Thermostat");

    printf("Proxy ready: 'Thermostat -e 1.0 -o:tcp -h 127.0.0.1 -p 7892'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

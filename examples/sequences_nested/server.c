/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/UDPEndpoint.h>

#include <stdio.h>
#include <math.h>

#include "sequences.h"


void
Example_VehicleI_sendCommand(Example_VehiclePtr this,
			     short id,
			     int opcode,
			     Example_ParamSeq params) {
    Example_Param p3;
    const char* p3_data;

    assert(params.size == 3);
    p3 = params.items[2];
    p3_data = (const char*)p3.data.items;

    printf("Vehicle: sendCommand:\n"
	   " - id: %d\n"
	   " - opcode: %d\n"
	   " - params.length: %d\n"
	   " - p3.id = %d\n"
	   " - p3.data = '%s'\n",
	   id, opcode, params.size, p3.id, p3_data);
    fflush(NULL);
}

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Vehicle servant;

    (void) argc;
    (void) argv;

    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "udp -h 127.0.0.1 -p 7892", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    Example_Vehicle_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "Vehicle");

    printf("Proxy ready: 'Vehicle -e 1.0 -d:udp -h 127.0.0.1 -p 7892'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

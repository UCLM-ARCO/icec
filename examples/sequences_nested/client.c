/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/UDPEndpoint.h>

#include <stdio.h>

#include "sequences.h"

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectPrx vehicle;

    short id;
    int opcode;
    Example_ParamSeq params;
    Example_Param params_data[3];

    if (argc < 2) {
	printf("USAGE: %s <proxy>\n", argv[0]);
	return -1;
    }

    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    Ice_Communicator_stringToProxy(&ic, argv[1], &vehicle);

    id = 123;
    opcode = 456;
    params.size = 3;
    params.items = params_data;

    params_data[0].id = 21;
    params_data[0].data.items = (Ice_Byte*)"\0\1\2";
    params_data[0].data.size = 3;

    params_data[1].id = 22;
    params_data[1].data.items = NULL;
    params_data[1].data.size = 0;

    params_data[2].id = 23;
    params_data[2].data.items = (Ice_Byte*)"a string";
    params_data[2].data.size = strlen((const char*)params_data[2].data.items);

    Example_Vehicle_sendCommand(&vehicle, id, opcode, params);

    return 0;
}

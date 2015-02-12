/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/UDPEndpoint.h>


typedef struct Example_StringDict_Pair {
    Ice_String key;
    Ice_String value;
} Example_StringDict_Pair;

typedef Example_StringDict_Pair* Example_StringDict_PairPtr;

typedef struct Example_StringDict {
    uint8_t size;
    Example_StringDict_PairPtr items;
} Example_StringDict;

#define Example_StringDict_set(DICT, INDEX, KEY, VALUE)		\
    Ice_String_init(DICT.items[INDEX].key, KEY);		\
    Ice_String_init(DICT.items[INDEX].value, VALUE);

#include "example.h"

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectPrx server;

    Example_StringDict props;
    Example_StringDict_Pair props_values[2];

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
    Dict_init(props, props_values, 2);
    Example_StringDict_set(props, 0, "name", "sensor");
    Example_StringDict_set(props, 1, "type", "humidity");

    Example_PropertyService_update(&server, props);
    return 0;
}

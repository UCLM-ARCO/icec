/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/platforms/x86/TCPEndpoint.h>
#include "hello.h"

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectPrx manager;
    Ice_ObjectPrx speaker;

    if (argc < 2)
	return printf("USAGE: %s <proxy>\n", argv[0]);

    /* create communicator */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* create proxy to server */
    Ice_Communicator_stringToProxy(&ic, argv[1], &manager);

    /* read invocation*/
    char* msg_data = "Speaker called properly";
    Ice_String msg;
    Ice_String_init(msg, msg_data);

    // FIXME: can't be used this way
    speaker = Example_Manager_getSpeaker(&manager);
    // Example_Manager_getSpeaker(&manager, &speaker);

    Example_Speaker_say(&speaker, msg);

    /* IMPORTANT! close each proxy connection */
    Ice_ObjectPrx_closeConnection(&manager);
    Ice_ObjectPrx_closeConnection(&speaker);
    return 0;
}

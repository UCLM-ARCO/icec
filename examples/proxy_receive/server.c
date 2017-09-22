/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/platforms/x86/TCPEndpoint.h>
#include "hello.h"

void
Example_SpeakerI_sayToSpeaker(Example_SpeakerPtr self,
                              Ice_ObjectPrxPtr s,
                              Ice_String msg) {
    Example_Speaker_say(s, msg);
}

void
Example_SpeakerI_say(Example_SpeakerPtr self,
		     Ice_String msg) {

    printf("%.*s\n", msg.size, msg.value);
    fflush(stdout);
}

int
main(int argc, char* argv[]) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_Speaker servant;

    /* create communicator */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* create adapter */
    char* endp = "tcp -h 127.0.0.1 -p 7903";
    Ice_Communicator_createObjectAdapterWithEndpoints(&ic, "Adapter", endp, &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_Speaker_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "Speaker");

    /* wait for events (event loop) */
    printf("Proxy ready: 'Speaker -e 1.0 -t:tcp -h 127.0.0.1 -p 7903'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

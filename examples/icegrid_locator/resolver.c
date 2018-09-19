/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

#include "Locator.h"

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectPrx manager;
    Ice_ObjectPrx topic;
    Ice_ObjectPrx publisher;

    /* init communicator and endpoint */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* set default locator */
    Ice_Communicator_setDefaultLocator
	(&ic, "TestIceGrid/Locator -t:tcp -h 127.0.0.1 -p 4061");

    /* get real proxy from indirect one */
    char* strprx = "TestApp.IceStorm/TopicManager -t:tcp -h 127.0.0.1 -p 7910";
    Ice_Communicator_stringToProxy(&ic, strprx, &manager);

    return 0;
}

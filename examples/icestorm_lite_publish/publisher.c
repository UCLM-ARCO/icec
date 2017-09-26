/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

#include "IceStorm-lite.h"
#include "example.h"

int
main(int argc, char** argv) {
    Ice_Communicator ic;

    /* init communicator and endpoint */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* get IceStorm manager proxy */
    char* strprx = "IceStorm/TopicManager -t:tcp -h 127.0.0.1 -p 7910";


    return 0;
}

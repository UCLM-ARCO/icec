/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

#include "IceStorm-lite.h"
#include "example.h"

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectPrx manager;
    Ice_ObjectPrx topic;
    Ice_ObjectPrx publisher;

    /* init communicator and endpoint */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* get IceStorm manager proxy */
    char* strprx = "IceStorm/TopicManager -t:tcp -h 127.0.0.1 -p 7910";
    Ice_Communicator_stringToProxy(&ic, strprx, &manager);

    /* retrieve proxy */
    new_Ice_String(topic_name, "HelloTopic");

    IceStorm_TopicManager_retrieve(&manager, topic_name, &topic);
    if (Ice_ObjectPrx_raisedException(&manager, "::IceStorm::NoSuchTopic")) {
	IceStorm_TopicManager_create(&manager, topic_name, &topic);
    }

    /* IMPORTANT! close each proxy connection */
    Ice_ObjectPrx_closeConnection(&manager);
    // Ice_ObjectPrx_closeConnection(&topic);
    // Ice_ObjectPrx_closeConnection(&publisher);
    return 0;
}

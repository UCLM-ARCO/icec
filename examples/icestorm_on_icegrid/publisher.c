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

    /* set default locator */
    Ice_Communicator_setDefaultLocator
	(&ic, "TestIceGrid/Locator -t:tcp -h 127.0.0.1 -p 4061");

    /* get IceStorm manager proxy */
    char* strprx = "TestApp.IceStorm/TopicManager -t:tcp -h 127.0.0.1 -p 7910";
    Ice_Communicator_stringToProxy(&ic, strprx, &manager);

    /* retrieve topic */
    new_Ice_String(topic_name, "HelloTopic");
    IceStorm_TopicManager_retrieve(&manager, topic_name, &topic);
    if (Ice_ObjectPrx_raisedException(&manager, "::IceStorm::NoSuchTopic")) {
	IceStorm_TopicManager_create(&manager, topic_name, &topic);
    }

    Ice_String result;
    result.size = 512;
    char buff[result.size];
    result.value = buff;

    /* print the retrieved proxies */
    Ice_Communicator_proxyToString(&ic, &manager, &result);
    printf("manager: '%s'\n", buff);
    Ice_Communicator_proxyToString(&ic, &topic, &result);
    printf("topic: '%s'\n", buff);

    /* retrieve topic's publisher */
    IceStorm_Topic_getPublisher(&topic, &publisher);
    Example_Hello_sayHello(&publisher);
    return 0;
}

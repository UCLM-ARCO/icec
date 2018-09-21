/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

#include "IceStorm-lite.h"

int
main() {
    Ice_Communicator ic;
    Ice_ObjectPrx manager;
    Ice_ObjectPrx topic;

    char buff[512];
    Ice_String strprx;
    strprx.size = 512;
    strprx.value = buff;

    /* init communicator and endpoint */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* set default locator */
    Ice_Communicator_setDefaultLocator
    	(&ic, "TestIceGrid/Locator -t:tcp -h 127.0.0.1 -p 4061");

    /* get indirect proxy from string */
    char* strmanager = "TestApp.IceStorm/TopicManager -t @ TestApp.IceStorm.TopicManager";
    Ice_Communicator_stringToProxy(&ic, strmanager, &manager);
    Ice_Communicator_proxyToString(&ic, &manager, &strprx);
    printf("indirect proxy: '%s'\n", buff);

    /* use the proxy to force its resolution */
    new_Ice_String(topic_name, "HelloTopic");
    IceStorm_TopicManager_retrieve(&manager, topic_name, &topic);
    if (Ice_ObjectPrx_raisedException(&manager, "::IceStorm::NoSuchTopic")) {
	IceStorm_TopicManager_create(&manager, topic_name, &topic);
    }

    /* print the resolved (direct) proxy */
    Ice_Communicator_proxyToString(&ic, &manager, &strprx);
    printf("resolved proxy: '%s'\n", buff);

    /* print the received proxy */
    Ice_Communicator_proxyToString(&ic, &topic, &strprx);
    printf("retrieved topic: '%s'\n", buff);

    return 0;
}

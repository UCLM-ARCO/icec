// -*- mode: cpp; coding: utf-8 -*-

#include <Arduino.h>

#include <ArduinoOTA.h>    // force IDE to include this
#include <ESP8266WiFi.h>   // force IDE to include this
#include <ESP8266mDNS.h>   // force IDE to include this
#include <EEPROM.h>        // force IDE to include this

#include <IceC.h>
#include <IceC/platforms/esp8266/TCPEndpoint.h>
#include <IceC/platforms/esp8266/debug.hpp>
#include <IceC-IoT-node.h>

#include "IceStorm-lite.h"
#include "example.h"

#define TOPIC_NAME "HelloTopic"

// IceC broker and adapter
Ice_Communicator ic;
Ice_ObjectAdapter adapter;

// proxies
Ice_ObjectPrx manager;
Ice_ObjectPrx topic;
Ice_ObjectPrx publisher;

bool
setup_publisher() {
    char* strprx = "TestApp.IceStorm/TopicManager -t @ TestApp.IceStorm.TopicManager";
    Ice_Communicator_stringToProxy(&ic, strprx, &manager);

    if (not get_topic(&manager, TOPIC_NAME, &topic))
        return false;
    if (not get_publisher(&topic, &publisher))
        return false;
    return true;
}

void setup() {
    Serial.begin(74880);
    Serial.flush();
    delay(100);
    Serial.println("\n----------\nBooting...\n");

    // setup WiFi, Ice, Endpoints...
    IceC_Storage_begin();
    setup_wireless();
    setup_ota();

    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    // set default locator
    Ice_Communicator_setDefaultLocator
    	(&ic, "TestIceGrid/Locator -t:tcp -h 192.168.4.2 -p 4061");

    // wait for user to press button 0
    Serial.println("Boot done!\n----------");
    Serial.println("Press button on NodeMCU when you're ready.");
}

static bool ready = false;
void loop() {
    if (!ready && digitalRead(PIN_BUTTON) == 0) {
        Serial.println("Ok, starting publisher...");

        // get publisher or reset if fail
        if (not setup_publisher()) {
            prints("ERROR: can't get IS publisher\n");
            reset_node();
        }

        Serial.println("Publisher ready!");
        // FIXME: setup timer to publish events
    }

    handle_ota();
    check_buttons();
    Ice_Communicator_loopIteration(&ic);
}








// int
// main(int argc, char** argv) {
//     Ice_Communicator ic;
//     Ice_ObjectPrx manager;
//     Ice_ObjectPrx topic;
//     Ice_ObjectPrx publisher;

//     char buff[512];
//     new_Ice_String(result, buff);
//     result.size = 512;

//     /* init communicator and endpoint */
//     Ice_initialize(&ic);
//     TCPEndpoint_init(&ic);

//     /* set default locator */
//     Ice_Communicator_setDefaultLocator
// 	(&ic, "TestIceGrid/Locator -t:tcp -h 127.0.0.1 -p 4061");

//     /* get IceStorm manager proxy */
//     char* strprx = "TestApp.IceStorm/TopicManager -t:tcp -h 127.0.0.1 -p 7910";
//     Ice_Communicator_stringToProxy(&ic, strprx, &manager);
//     Ice_Communicator_proxyToString(&ic, &manager, &result);
//     printf("manager: '%s'\n", buff);

//     /* retrieve topic */
//     new_Ice_String(topic_name, "HelloTopic");
//     IceStorm_TopicManager_retrieve(&manager, topic_name, &topic);
//     if (Ice_ObjectPrx_raisedException(&manager, "::IceStorm::NoSuchTopic")) {
// 	IceStorm_TopicManager_create(&manager, topic_name, &topic);
//     }
//     if (Ice_ObjectPrx_raisedAnyException(&manager)) {
// 	printf("ERROR: could not create/retrieve topic!!");
// 	return 1;
//     }

//     /* print the retrieved proxies */
//     Ice_Communicator_proxyToString(&ic, &topic, &result);
//     printf("topic: '%s'\n", buff);

//     /* retrieve topic's publisher */
//     IceStorm_Topic_getPublisher(&topic, &publisher);
//     Example_Hello_sayHello(&publisher);
//     return 0;
// }

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

Ticker timer;

bool
setup_publisher() {
    char* strprx = "TestApp.IceStorm/TopicManager -t @ TestApp.IceStorm.TopicManager";
    Ice_Communicator_stringToProxy(&ic, strprx, &manager);

    char buff[256];
    new_Ice_String(prx, buff);
    prx.size = 256;
    Ice_Communicator_proxyToString(&ic, &manager, &prx);
    printf(" -- Initial Manager Prxoy: '%s'\n", buff);

    if (not get_topic(&manager, TOPIC_NAME, &topic))
        return false;

    Ice_Communicator_proxyToString(&ic, &manager, &prx);
    printf(" -- Resolved Manager Prxoy: '%s'\n", buff);
    Ice_Communicator_proxyToString(&ic, &topic, &prx);
    printf(" -- Retrieved Topic Prxoy: '%s'\n", buff);

    if (not get_publisher(&topic, &publisher))
        return false;
    return true;
    return false;
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
    Serial.println("Now, connect to the ESP AP, and launch IceGrid and IceStorm.");
    Serial.println("Press button on NodeMCU when you're ready.");
}

static bool ready = false;
static bool should_send = false;
void loop() {
    if (!ready && digitalRead(PIN_BUTTON) == 0) {
        Serial.println("Ok, starting publisher...");
        ready = true;

        // get publisher or reset if fail
        if (not setup_publisher()) {
            prints("ERROR: can't get IS publisher\n");
            reset_node();
        }

        Serial.println("Publisher ready!");
        timer.attach(1, []() {
            // this will probably use a ISR, so be as brief as possible
            should_send = true;
        });
    }

    if (should_send) {
        should_send = false;
        Example_Hello_sayHello(&publisher);
        printf("saying hello!\n");
    }

    handle_ota();
    check_buttons();
    Ice_Communicator_loopIteration(&ic);
}


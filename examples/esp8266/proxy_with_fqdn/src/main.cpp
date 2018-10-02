// -*- mode: c++; coding: utf-8 -*-

#include <Arduino.h>

#include <ArduinoOTA.h>    // force IDE to include this
#include <ESP8266WiFi.h>   // force IDE to include this
#include <ESP8266mDNS.h>   // force IDE to include this
#include <EEPROM.h>        // force IDE to include this

#include <IceC/IceC.h>
#include <IceC/platforms/esp8266/TCPEndpoint.h>
#include <IceC/platforms/esp8266/debug.hpp>
#include <IceC-IoT-node.h>
#include <IceC/Locator.h>

#include "IceObject.h"

// IceC and proxies
Ice_Communicator ic;
Ice_ObjectPrx locator;

// Timer to periodically send 'hellos'
Ticker timer;


void get_ice_id() {
    Ice_String ice_id = Ice_ObjectIface_ice_id(&locator);
    printf("Retrieved ice_id: '%s'\n", ice_id.value);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n----------\nBooting...\n");

    // setup WiFi, Ice, Endpoints...
    IceC_Storage_begin();
    setup_wireless();
    setup_ota();

    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    // create proxy to locator
    Ice_Communicator_stringToProxy
        (&ic, "IceGrid/Locator -t:tcp -h pike.esi.uclm.es -p 5061", &locator);

    Serial.println("Boot done!\n----------\n");
}

unsigned long t1 = millis();
void loop() {
    if (millis() - t1 > 3000) {
        t1 = millis();
        get_ice_id();
    }

    handle_ota();
    check_buttons();
    Ice_Communicator_loopIteration(&ic);
}

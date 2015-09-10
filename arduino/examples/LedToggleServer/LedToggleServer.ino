// -*- mode: c++; coding: utf-8 -*-

#include <ESP8266WiFi.h>
#include <IceC.h>
#include <IceC/platforms/esp8266/debug.hpp>
#include <TCPEndpoint.h>

#include "ap_creds.h"
#include "led_iface.h"

const char* ssid = SSID;
const char* key = KEY;

Ice_Communicator ic;
Ice_ObjectAdapter adapter;
Example_Led servant;

void
Example_LedI_on(Example_LedPtr self) {
    Serial.println("\n\nExample LED on\n");
}

void
Example_LedI_off(Example_LedPtr self) {
    Serial.println("\n\nExample LED off\n");
}

void
setup() {
    Serial.begin(115200);

    Serial.print("\n\nConnecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, key);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\nWiFi connected, IP address: ");
    Serial.println(WiFi.localIP());

    /* create communicator and setup endpoint */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* create object adapter */
    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "tcp -p 4455", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    Example_Led_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "Led");

    /* wait for events (event loop) */
    Serial.print("Proxy ready: 'Led -e 1.0 -o:tcp -h ");
    Serial.print(WiFi.localIP());
    Serial.println(" -p 4455'");
}

void
loop() {
    Ice_Communicator_loopIteration(&ic);
}

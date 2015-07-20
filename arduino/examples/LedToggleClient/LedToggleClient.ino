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
Ice_ObjectPrx led;
bool state = false;

void setup() {
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

    /* create proxy to server */
    const char* strprx = "Led -o:tcp -h 192.168.0.216 -p 4455";
    Ice_Communicator_stringToProxy(&ic, strprx, &led);
}

void loop() {
    /* invoke it */
    if (state)
    	Example_Led_on(&led);
    else
    	Example_Led_off(&led);

    state = !state;
    delay(1000);
}

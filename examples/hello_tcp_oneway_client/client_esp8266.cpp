// -*- mode: c++; coding: utf-8 -*-

#include <uncle.h>
#include <IceC/IceC.h>

void
setup() {
    Serial.begin(115200);
    delay(1000);

    // const char* ssid = CONST_VALUE(WIFI_SSID);
    // const char* pass = CONST_VALUE(WIFI_KEY);
    // WiFi.begin(ssid, pass);
    // os_printf("Connecting to '%s'...\n", ssid);
}

bool
loop() {
    // if (not WiFi.isReady()) {
    // 	delay(100);
    // 	return true;
    // }

    Ice_Communicator ic;
    Ice_initialize(&ic);

    Ice_ObjectPrx remote;
    Ice_Communicator_stringToProxy
     	(&ic, "'WriterServer -o:tcp -h 192.168.1.128 -p 7893", &remote);

    return false;
}

// -*- mode: c++; coding: utf-8 -*-

#include <uncle.h>
#include <hello.h>
#include <IceC/platforms/esp8266/TCPEndpoint.h>


void ICACHE_FLASH_ATTR
setup() {
    Serial.begin(115200);
    delay(1000);

    // const char* ssid = CONST_VALUE(WIFI_SSID);
    // const char* pass = CONST_VALUE(WIFI_KEY);
    // WiFi.begin(ssid, pass);
    // os_printf("Connecting to '%s'...\n", ssid);
}

bool ICACHE_FLASH_ATTR
loop() {
    // if (not WiFi.isReady()) {
    // 	 delay(100);
    // 	 return true;
    // }

    Ice_Communicator ic;
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    Ice_ObjectPrx writer;
    Ice_Communicator_stringToProxy
     	(&ic, "'WriterServer -o:tcp -h 192.168.1.128 -p 7893", &writer);

    Example_Writer_write(&writer, 1551);

    Ice_ObjectPrx_closeConnection(&writer);
    return false;
}

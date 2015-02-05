/* -*- mode: c++; coding: utf-8 -*- */

#include <IceC/platforms/arduino/UDPIO.h>
#include <Ethernet.h>

EthernetUDP udp;

void
Ethernet_setup() {
    byte mac[] = {MAC_ADDRESS};
    byte ip[] = {IP_ADDRESS};
    Ethernet.begin(mac, ip);
}

uint8_t
UDP_init(uint16_t port) {
    if (! udp.begin(port)) {
	throw("UDP_BeginError", __FILE__, __LINE__);
	return 1;
    }
    return 0;
}

uint8_t
UDP_read(byte* data, uint8_t size) {
    return udp.read(data, size);
}

bool
UDP_available() {
    udp.parsePacket();
    return udp.available() > 0;
}

/* -*- mode: c++; coding: utf-8 -*- */

#include <IceC/platforms/arduino/UDPIO.h>
#include <Ethernet.h>

EthernetUDP udp;

uint8_t
UDP_read(byte* data, uint8_t size) {
    return udp.read(data, size);
}

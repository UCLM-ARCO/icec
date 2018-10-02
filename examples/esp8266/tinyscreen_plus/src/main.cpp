// -*- mode: c++; coding: utf-8 -*-

#include <SPI.h>
#include <Wire.h>
#include <TinyScreen.h>
#include <IceC/IceC.h>
#include <SerialEndpoint.h>

#include "iot.h"
#include "idm.h"

#define IDM_ADDR   "A00A0002"
#define IDM_ROUTER "A00A0001"
#define SERIAL_EPS "-h 127.0.0.1 -p 1793"

Ice_Communicator ic;
Ice_ObjectAdapter adapter;
Ice_ObjectPrx router;
SmartObject_DataSink servant;

TinyScreen display = TinyScreen(TinyScreenPlus);

void SmartObject_DataSinkI_notify(SmartObject_DataSinkPtr self,
                				  SmartObject_ByteSeq data,
				                  Ice_String source,
				                  SmartObject_Metadata meta) {
    display.clearScreen();
    display.drawRect(0, 0, 96, 64, TSRectangleFilled, TS_8b_White);
    display.setX(0,95);
    display.setY(0,63);
    display.startData();
    display.writeBuffer(data.items, data.size);
    display.endTransfer();
}

void send_adv() {
    new_Ice_String(ice_strprx, IDM_ADDR " -d:serial " SERIAL_EPS);
    IDM_NeighborDiscovery_Listener_adv(&router, ice_strprx);
}

void setup() {
    /* initialize IceC and endpoints */
    Ice_initialize(&ic);
    SerialEndpoint_init(&ic);
    delay(5000);

    /* create object adapter */
    Ice_Communicator_createObjectAdapterWithEndpoints
	    (&ic, "Adapter", "serial", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    /* register servant */
    SmartObject_DataSink_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, IDM_ADDR);

    /* create proxy to router, for advs */
    Ice_Communicator_stringToProxy(&ic, IDM_ROUTER " -d:serial", &router);
    send_adv();

    /* initialize other hardware components */
    display.begin();
    display.setBrightness(10);
}

unsigned long t1 = millis();
void loop() {
    // DEBUG: remove on production
    // if (millis() - t1 > 3000) {
    //     t1 = millis();
	//     send_adv();
    // }

    /* as frequent as possible */
    Ice_Communicator_loopIteration(&ic);
}


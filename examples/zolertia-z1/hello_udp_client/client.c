/* -*- mode: c; coding: utf-8 -*- */

#include <contiki.h>
#include <dev/leds.h>
#include <stdio.h>

#include <IceC/IceC.h>
#include <IceC/platforms/z1/UDPEndpoint.h>
#include <hello.h>

PROCESS(icec_client, "IceC client");
AUTOSTART_PROCESSES(&icec_client);

PROCESS_THREAD(icec_client, ev, data) {
    PROCESS_EXITHANDLER(goto exit);
    PROCESS_BEGIN();

    {
/* -------------------------------------------------- */

	Ice_Communicator ic;
	Ice_ObjectPrx remote;

	Ice_initialize(&ic);
	UDPEndpoint_init(&ic);

	char* strprx = "Sample -d:udp -h fe80:0:0:0:baca:3aff:fe81:44fb -p 9876";
	Ice_Communicator_stringToProxy(&ic, strprx, &remote);

	while(1) {
	    leds_toggle(LEDS_BLUE);
	    clock_wait(CLOCK_SECOND);
	    Example_Writer_write(&remote, 123);
	}

/* -------------------------------------------------- */
    }

 exit:;
    PROCESS_END();
}

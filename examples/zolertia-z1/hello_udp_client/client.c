/* -*- mode: c; coding: utf-8 -*- */

#include <contiki.h>
#include <dev/leds.h>
#include <stdio.h>

PROCESS(blink_process, "blink example");
AUTOSTART_PROCESSES(&blink_process);

PROCESS_THREAD(blink_process, ev, data) {
    PROCESS_EXITHANDLER(goto exit);
    PROCESS_BEGIN();

    {
/* -------------------------------------------------- */

	while(1) {
	    leds_toggle(LEDS_BLUE);
	    clock_wait(CLOCK_SECOND);
	}

/* -------------------------------------------------- */
    }

 exit:;
    PROCESS_END();
}

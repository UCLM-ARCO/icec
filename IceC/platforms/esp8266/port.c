/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifdef ARDUINO_ARCH_ESP8266

#include <IceC/IceC.h>

void
IceInternal_selectOnAdapters(Ice_ObjectAdapter_ListPtr adapters,
			     Ice_ObjectAdapterPtr result[],
			     bool block) {
    int i, count;
    bool ready;
    Ice_ObjectAdapterPtr adapter;

    // trace();

    for (i=0; i<MAX_NUM_OF_ADAPTERS; i++)
    	result[i] = NULL;

    count = 0;
    do {

    	for (i=0; i<adapters->count; i++) {
    	    adapter = *(adapters->begin + i);
    	    if (! adapter->activated) {
    		continue;
    	    }

    	    ready = false;
    	    IcePlugin_Connection_dataReady(&(adapter->connection), &ready);
    	    if (ready)
    		result[count++] = adapter;
    	}
    } while (! count && block);
}

#endif /* ARDUINO_ARCH_ESP8266 */

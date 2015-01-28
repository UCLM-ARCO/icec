/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#include <IceC/IceC.h>

void
IceInternal_selectOnAdapters(Ice_ObjectAdapter_ListPtr adapters,
			     Ice_ObjectAdapterPtr result[]) {

    int i, count;
    bool ready;
    Ice_ObjectAdapterPtr adapter;

    trace();

    for (i=0; i<MAX_NUM_OF_ADAPTERS; i++)
    	result[i] = NULL;

    count = 0;
    while (! count) {

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
    }
}

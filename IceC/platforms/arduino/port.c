/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAMD)

#include <IceC/IceC.h>

void ICEC_FUNC_ATTR
IceInternal_selectOnAdapters(Ice_ObjectAdapter_ListPtr adapters,
                             Ice_ObjectAdapterPtr result[],
                             bool block) {

    int i, count;
    bool ready;
    Ice_ObjectAdapterPtr adapter;

    trace();

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

void ICEC_FUNC_ATTR
IceUtil_delay(uint16_t ms) {
    delay(ms);
}

#endif /* ARDUINO_ARCH_AVR */

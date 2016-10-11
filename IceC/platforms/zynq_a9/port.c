/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#include "FreeRTOS.h"
#include "FreeRTOS_Sockets.h"
#include <IceC/IceC.h>

#include "TCPEndpoint.h"

void
IceInternal_selectOnAdapters(Ice_ObjectAdapter_ListPtr adapters,
			     Ice_ObjectAdapterPtr result[],
			     bool block) {
    int i, count;
    Ice_ObjectAdapterPtr adapter;
    SocketSet_t xFD_Set;
    Socket_t socket_map[MAX_NUM_OF_ADAPTERS];
    Ice_ObjectAdapterPtr adapter_map[MAX_NUM_OF_ADAPTERS];

    trace();

    xFD_Set = FreeRTOS_CreateSocketSet();

    count = 0;
	for (i=0; i<adapters->count; i++) {
	    adapter = *(adapters->begin + i);
	    if (! adapter->activated) {
	    	continue;
	    }

	    /* Add the created socket to the set. */
	    TCP_EndpointOptionsPtr options = ((TCP_EndpointOptionsPtr)(adapter->connection.epinfo->options));
	    Socket_t socket = (adapter->connection.fd == 1)?options->connected_socket:options->socket;
	    FreeRTOS_FD_SET( socket, xFD_Set, eSELECT_READ);
	    socket_map[count] = socket;
	    adapter_map[count] = adapter;
		count++;
	}

    if ( (FreeRTOS_select( xFD_Set, portMAX_DELAY ))!= 0) {


    	for (i=0; i<MAX_NUM_OF_ADAPTERS; i++)
    		result[i] = NULL;

    	for (i=0; i<count; i++) {
    		BaseType_t event = FreeRTOS_FD_ISSET(socket_map[i], xFD_Set);
    		if (event) {
    			result[i] = adapter_map[i];
    			break;
    		}
    	}
    }

}


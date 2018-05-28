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
#ifdef DEBUG
	FreeRTOS_printf( ( "[DBG-PORT] Network buffers: %lu (current free) lowest %lu (history free)\n",
			uxGetNumberOfFreeNetworkBuffers(), uxGetMinimumFreeNetworkBuffers() ) );
#endif

    count = 0;
	for (i=0; i<adapters->count; i++) {
	    adapter = *(adapters->begin + i);
	    if (! adapter->activated) {
	    	continue;
	    }
	    Socket_t socket;
	    TCP_UDPEndpointOptions *options;

	    /* Add the created socket to the set. */
	    options = ((TCP_UDPEndpointOptions*)(adapter->connection.epinfo->options));
	    if (adapter->connection.epinfo->type == Ice_EndpointTypeTCP) {
	      socket = (options->connected_socket != NULL)?options->connected_socket:options->socket;
	    }
	    else {
	      socket = options->socket;
	    }
	    xFD_Set = options->socket_set;
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

void
IceUtil_delay(uint16_t ms) {
	vTaskDelay (ms / portTICK_RATE_MS);
}

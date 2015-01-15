/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/UDPEndpoint.h>

#include <stdio.h>
#include <math.h>

#include "sequences.h"

void
Example_VideoFrameI_showImage(Example_VideoFramePtr this,
			      Example_Size size,
			      Example_PixelSeq img) {
    int i, cols;

    printf("VideoFrame: showImage, size: %dx%d, size: %d\n",
	   size.width, size.height, img.size);

    cols = sqrt(img.size);
    for(i=0; i<img.size; i++) {
	Example_PixelPtr p = &img.items[i];
	printf("%03d,%d,%d|", p->R, p->G, p->B);
	if ((i+1) % cols == 0)
	    printf("\b \n");
    }

    fflush(NULL);
}

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectAdapter adapter;
    Example_VideoFrame servant;

    (void) argc;
    (void) argv;

    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    Ice_Communicator_createObjectAdapterWithEndpoints
	(&ic, "Adapter", "udp -h 127.0.0.1 -p 7892", &adapter);
    Ice_ObjectAdapter_activate(&adapter);

    Example_VideoFrame_init(&servant);
    Ice_ObjectAdapter_add(&adapter, (Ice_ObjectPtr)&servant, "VideoFrame");

    printf("Proxy ready: 'VideoFrame -e 1.0 -d:udp -h 127.0.0.1 -p 7892'\n");
    fflush(NULL);

    Ice_Communicator_waitForShutdown(&ic);
    return 0;
}

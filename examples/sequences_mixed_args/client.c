/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/UDPEndpoint.h>

#include <stdio.h>

#include "sequences.h"

#define IMG_SIZE 16

int
main(int argc, char** argv) {
    Ice_Communicator ic;
    Ice_ObjectPrx frame;
    Example_Pixel data[IMG_SIZE * IMG_SIZE];

    if (argc < 2) {
	printf("USAGE: %s <proxy>\n", argv[0]);
	return -1;
    }

    /* a simple image */ {
	int i;
	for(i=0; i<IMG_SIZE * IMG_SIZE; i++) {
	    data[i].R = i % 256;
	    data[i].G = 0;
	    data[i].B = 0;
	}
    }

    Ice_initialize(&ic);
    UDPEndpoint_init(&ic);

    Ice_Communicator_stringToProxy(&ic, argv[1], &frame);

    /* send image to frame */ {
	Example_PixelSeq image;
	int ts = 1;

	image.items = data;
	image.size = IMG_SIZE * IMG_SIZE;
	Example_VideoFrame_showImage(&frame, ts, image);
    }

    return 0;
}

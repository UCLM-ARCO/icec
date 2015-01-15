// -*- mode: c++; coding: utf-8 -*-

module Example {

    struct Pixel {
	byte R;
	byte G;
	byte B;
    };

    sequence<Pixel> PixelSeq;

    interface VideoFrame {
	void showImage(int timestamp, PixelSeq img);
    };
};

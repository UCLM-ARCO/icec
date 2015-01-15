// -*- mode: c++; coding: utf-8 -*-

module Example {

    struct Pixel {
	byte R;
	byte G;
	byte B;
    };

    struct Size {
	int width;
	int height;
    };

    sequence<Pixel> PixelSeq;

    interface VideoFrame {
	void showImage(Size s, PixelSeq img);
    };
};

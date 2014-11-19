// -*- mode: c++; coding: utf-8 -*-

module Example {
    interface Writer {
	void write(int code);
    };

    interface Reader {
	int read();
    };
};

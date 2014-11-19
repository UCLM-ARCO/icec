// -*- mode: c++; coding: utf-8 -*-

module Example {
    interface Writer {
	void write(string message);
    };

    interface Reader {
	string read();
    };
};

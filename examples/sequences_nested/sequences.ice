// -*- mode: c++; coding: utf-8 -*-

module Example {

    sequence<byte> ByteSeq;

    struct Param {
	int id;
	ByteSeq data;
    };

    sequence<Param> ParamSeq;

    interface Vehicle {
	void sendCommand(short id, int opcode, ParamSeq params);
    };
};

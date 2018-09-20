// -*- mode: c++; coding: utf-8 -*-

#include <Ice/Identity.ice>

module Example {

    interface Hello {
	void sayHello(Ice::Identity oid);
    };
};

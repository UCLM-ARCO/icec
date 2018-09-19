// -*- mode: c++; coding: utf-8 -*-

#pragma once

#include <Ice/Identity.ice>

// FIXME: test with the full interface, maybe it works...

module Ice {

    exception AdapterNotFoundException {};
    exception ObjectNotFoundException {};

    interface Locator {

	Object* findObjectById(Ice::Identity id)
	    throws ObjectNotFoundException;

	Object* findAdapterById(string id)
	    throws AdapterNotFoundException;
    };
};

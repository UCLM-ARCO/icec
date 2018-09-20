/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

#include "example.h"

int
main() {
    Ice_Communicator ic;
    Ice_ObjectPrx server;

    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    Ice_Communicator_stringToProxy(&ic, "Server -t:tcp -h 127.0.0.1 -p 1234", &server);

    Ice_Identity oid;
    new_Ice_String(name, "myObject");
    oid.name = name;
    new_Ice_String(category, "myCategory");
    oid.category = category;

    Example_Hello_sayHello(&server, oid);
    return 0;
}

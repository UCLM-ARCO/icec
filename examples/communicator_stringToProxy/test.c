/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/platforms/x86/TCPEndpoint.h>

int
main() {
    Ice_Communicator ic;
    Ice_ObjectPrx proxy;

    /* init communicator and endpoint */
    Ice_initialize(&ic);
    TCPEndpoint_init(&ic);

    /* create proxy from string */
    char* strprx = "Object1 -t:tcp -h 127.0.0.1 -p 1234";
    Ice_Communicator_stringToProxy(&ic, strprx, &proxy);

    /* create string from proxy */
    Ice_String result;
    result.size = 512;
    char buff[result.size];
    result.value = buff;
    Ice_Communicator_proxyToString(&ic, &proxy, &result);

    /* print it and exit */
    printf("Proxy: '%s'\n", result.value);
    printf("Everything is fine!\n");
    return 0;
}

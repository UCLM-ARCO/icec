#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("example.ice -I/usr/share/slice --all")
import Example  # noqa


class HelloI(Example.Hello):
    def sayHello(self, oid, current):
        print(" - oid: {}".format(Ice.identityToString(oid)))


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "tcp -h 127.0.0.1 -p 1234")
        adapter.activate()

        oid = ic.stringToIdentity("Server")
        proxy = adapter.add(HelloI(), oid)
        proxy = proxy.ice_encodingVersion(Ice.Encoding_1_0)

        print(("Proxy ready: '{0}'".format(proxy)))
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

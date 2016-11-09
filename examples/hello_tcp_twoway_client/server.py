#!/usr/bin/python -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("hello.ice")
import Example

class ReaderI(Example.Reader):
    def read(self, current):
        print " - 'read' request, returns 1999"
        return 1999
        

class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "tcp -p 7893")
        adapter.activate()

        oid = ic.stringToIdentity("ReaderServer")
        proxy = adapter.add(ReaderI(), oid)
        proxy = proxy.ice_encodingVersion(Ice.Encoding_1_0)

        print "Proxy ready: '{0}'".format(proxy)
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

#!/usr/bin/python
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("slice.ice")
import Example


class WriterI(Example.Writer):
    def write(self, message, current):
        print "Write to server: '{0}'".format(message)


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "udp -h 127.0.0.1 -p 7890")
        adapter.activate()

        oid = ic.stringToIdentity("WriterServer")
        proxy = adapter.add(WriterI(), oid)
        proxy = proxy.ice_datagram()
        proxy = proxy.ice_encodingVersion(Ice.Encoding_1_0)

        print "Proxy ready: '{0}'".format(proxy)
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

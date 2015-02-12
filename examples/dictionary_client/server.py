#!/usr/bin/python -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("example.ice")
import Example


class PropertyServiceI(Example.Hello):
    def update(self, props, current):
        print " - 'update' request, {}".format(props)


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "udp -h 127.0.0.1 -p 7897")
        adapter.activate()

        proxy = adapter.add(PropertyServiceI(), ic.stringToIdentity("PropertyServer"))

        print "Proxy ready: '{}'".format(proxy)
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

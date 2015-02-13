#!/usr/bin/python -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("example.ice")
import Example


class PropertyServiceI(Example.PropertyService):
    def update(self, props, current):
        items = props.items()
        items.sort()
        props = "{"
        for k, v in items:
            props += "'{}': '{}', ".format(k, v)
        props = props[:-2] + "}"
        print " - 'update' request, {}".format(props)


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "udp -h 127.0.0.1 -p 7897")
        adapter.activate()

        proxy = adapter.add(PropertyServiceI(), ic.stringToIdentity("PropertyServer"))
        proxy = proxy.ice_datagram()

        print "Proxy ready: '{}'".format(proxy)
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

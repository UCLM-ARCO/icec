#!/usr/bin/env python
# -*- coding: utf-8; mode: python; -*-

import sys
import Ice

Ice.loadSlice("led_iface.ice")
import Example


class LedI(Example.Led):
    def on(self, current):
        print "ON called"

    def off(self, current):
        print "OFF called"


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "tcp -h 192.168.0.216 -p 4455")
        adapter.activate()

        proxy = adapter.add(LedI(), ic.stringToIdentity("Led"))
        print "Proxy ready: '{}'".format(proxy)

        self.shutdownOnInterrupt()
        ic.waitForShutdown()

if __name__ == '__main__':
    Server().main(sys.argv)

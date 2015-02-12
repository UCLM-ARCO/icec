#!/usr/bin/python -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("example.ice")
import Example


class HelloI(Example.Hello):
    def sayHelloFrom(self, name, times, current):
        print " - 'sayHelloFrom' request, from: '{}', times: {}".format(name, times)


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "udp -h 127.0.0.1 -p 7896")
        adapter.activate()

        proxy = adapter.add(HelloI(), ic.stringToIdentity("HelloServer"))

        print "Proxy ready: '{}'".format(proxy)
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

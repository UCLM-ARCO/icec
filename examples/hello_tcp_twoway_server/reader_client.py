#!/usr/bin/python -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("hello.ice")
import Example


class Client(Ice.Application):
    def run(self, args):
        if len(args) < 2:
            return self.usage()

        ic = self.communicator()
        server = ic.stringToProxy(args[1])
        server = Example.ReaderPrx.uncheckedCast(server)

        code = server.read()
        print "Read from server: '{0}'".format(code)

    def usage(self):
        print "USAGE: {0} <proxy>".format(sys.argv[0])


if __name__ == '__main__':
    Client().main(sys.argv)

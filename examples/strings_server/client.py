#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("example.ice")
import Example


class Client(Ice.Application):
    def run(self, args):
        if len(args) < 2:
            print(("USAGE: {} <proxy>".format(args[0])))
            return 1

        ic = self.communicator()

        proxy = ic.stringToProxy(args[1])
        proxy = Example.HelloPrx.uncheckedCast(proxy)
        proxy.sayHelloFrom("Jonh Doe", 5)


if __name__ == '__main__':
    Client().main(sys.argv)

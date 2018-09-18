#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("example.ice")
import Example


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        if len(args) < 2:
            print(("USAGE: {} <proxy>".format(args[0])))
            return 1

        ic = self.communicator()

        proxy = ic.stringToProxy(args[1])
        proxy = Example.PropertyServicePrx.uncheckedCast(proxy)

        props = {'name': 'sensor', 'type': 'humidity'}
        proxy.update(props)


if __name__ == '__main__':
    Server().main(sys.argv)

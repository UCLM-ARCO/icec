#!/usr/bin/env python
# -*- coding: utf-8; mode: python; -*-

import sys
import Ice

Ice.loadSlice("led_iface.ice")
import Example


class Client(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        proxy = ic.stringToProxy(args[1])
        proxy = Example.LedPrx.uncheckedCast(proxy)

        if args[2] == "0":
            proxy.off()
        else:
            proxy.on()

        import time
        time.sleep(1)
        proxy.off()


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage: {} <proxy> <0|1>".format(sys.argv[0])
        exit(1)

    Client().main(sys.argv)

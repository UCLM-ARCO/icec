#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("floats.ice")
import Example


class Client(Ice.Application):
    def run(self, args):
        if len(args) < 2:
            print(("Usage: {} <proxy> <temp>".format(args[0])))
            return 1

        temp = 25.6
        if len(args) > 2:
            temp = float(args[2])

        ic = self.communicator()
        server = ic.stringToProxy(args[1])
        server = server.ice_encodingVersion(Ice.Encoding_1_0)
        server = Example.ThermostatPrx.uncheckedCast(server)

        print(("Using thermostat: '{}'".format(server)))
        server.setTemperature(temp)
        print(("Temperature set to {}".format(temp)))


if __name__ == "__main__":
    Client().main(sys.argv)

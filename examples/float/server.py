#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("floats.ice")
import Example


class ThermostatI(Example.Thermostat):
    def setTemperature(self, value, current):
        print("setTemperature to {} C".format(value))


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "tcp -h 127.0.0.1 -p 7892")
        adapter.activate()

        proxy = adapter.add(ThermostatI(), ic.stringToIdentity("Thermostat"))
        proxy = proxy.ice_encodingVersion(Ice.Encoding_1_0)
        print("Proxy ready: '{}'".format(proxy))

        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == "__main__":
    Server().main(sys.argv)

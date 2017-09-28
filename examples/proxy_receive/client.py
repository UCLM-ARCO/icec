#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("hello.ice")
import Example  # noqa


class SpeakerI(Example.Speaker):
    def say(self, msg, current):
        print(msg)


class Client(Ice.Application):
    def run(self, args):
        if len(args) != 2:
            print("Usage: {} <proxy>".format(args[0]))
            return 1

        ic = self.communicator()
        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "tcp -h 127.0.0.1")
        adapter.activate()

        cb = adapter.addWithUUID(SpeakerI())

        server = ic.stringToProxy(args[1])
        server = server.ice_encodingVersion(Ice.Encoding_1_0)
        server = Example.SpeakerPrx.uncheckedCast(server)

        server.sayToSpeaker(cb, "Speaker called properly")
        print("Done.")


if __name__ == '__main__':
    Client().main(sys.argv)

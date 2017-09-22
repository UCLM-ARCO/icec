#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("hello.ice")
import Example  # noqa


class Client(Ice.Application):
    def run(self, args):
        if len(args) != 2:
            print("Usage: {} <proxy>".format(args[0]))
            return 1

        ic = self.communicator()
        server = ic.stringToProxy(args[1])
        server = server.ice_encodingVersion(Ice.Encoding_1_0)
        server = Example.SpeakerPrx.uncheckedCast(server)

        server.sayToSpeaker(server, "Speaker called properly")
        print("Done.")


if __name__ == '__main__':
    Client().main(sys.argv)

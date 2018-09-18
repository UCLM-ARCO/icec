#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("hello.ice")
import Example


class SpeakerI(Example.Speaker):
    def sayToSpeaker(self, s, msg, current):
        s = Example.SpeakerPrx.uncheckedCast(s)
        s.say(msg)

    def say(self, msg, current):
        print(msg)


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "tcp -h 127.0.0.1 -p 7902")
        adapter.activate()

        oid = ic.stringToIdentity("Speaker")
        proxy = adapter.add(SpeakerI(), oid)
        proxy = proxy.ice_encodingVersion(Ice.Encoding_1_0)

        print(("Proxy ready: '{0}'".format(proxy)))
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice

Ice.loadSlice("hello.ice")
import Example  # noqa


class SpeakerI(Example.Speaker):
    def say(self, msg, current):
        print(msg)


class ManagerI(Example.Manager):
    def __init__(self, speaker):
        speaker = speaker.ice_encodingVersion(Ice.Encoding_1_0)
        self.speaker = Example.SpeakerPrx.uncheckedCast(speaker)

    def getSpeaker(self, current):
        print('getSepaker called')
        return self.speaker


class Server(Ice.Application):
    def run(self, args):
        ic = self.communicator()

        adapter = ic.createObjectAdapterWithEndpoints(
            "Adapter", "tcp -h 127.0.0.1 -p 7904")
        adapter.activate()

        speaker = adapter.addWithUUID(SpeakerI())
        oid = ic.stringToIdentity("Manager")
        proxy = adapter.add(ManagerI(speaker), oid)
        proxy = proxy.ice_encodingVersion(Ice.Encoding_1_0)

        print("Manager ready: '{0}'".format(proxy))
        self.shutdownOnInterrupt()
        ic.waitForShutdown()


if __name__ == '__main__':
    Server().main(sys.argv)

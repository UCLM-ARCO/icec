#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice
import IceStorm

Ice.loadSlice('example.ice')
import Example  # noqa


class HelloI(Example.Hello):
    def sayHello(self, current):
        print('Hello from IceStorm!')


class Subscriber(Ice.Application):
    def run(self, args):
        ic = self.communicator()
        locator = ic.stringToProxy(
            "TestIceGrid/Locator -t:tcp -h 192.168.4.2 -p 4061")
        locator = Ice.LocatorPrx.checkedCast(locator)
        ic.setDefaultLocator(locator)

        adapter = ic.createObjectAdapterWithEndpoints('Adapter', 'tcp')
        adapter.activate()

        proxy = adapter.addWithUUID(HelloI())
        topic = self.get_topic('HelloTopic')
        topic.subscribeAndGetPublisher({}, proxy)

        print('OK, waiting events...')
        self.shutdownOnInterrupt()
        ic.waitForShutdown()

    def get_topic(self, name):
        ic = self.communicator()
        mgr = 'TestApp.IceStorm/TopicManager -t @ TestApp.IceStorm.TopicManager'
        mgr = ic.stringToProxy(mgr)
        mgr = IceStorm.TopicManagerPrx.checkedCast(mgr)

        try:
            return mgr.retrieve(name)
        except IceStorm.NoSuchTopic:
            return mgr.create(name)


if __name__ == '__main__':
    Subscriber().main(sys.argv)

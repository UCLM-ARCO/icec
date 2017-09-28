#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import Ice
import IceStorm

Ice.loadSlice('example.ice')
import Example  # noqa


class Publisher(Ice.Application):
    def run(self, args):
        topic = self.get_topic('HelloTopic')
        pub = topic.getPublisher().ice_encodingVersion(Ice.Encoding_1_0)
        pub = Example.HelloPrx.uncheckedCast(pub)

        pub.sayHello()

    def get_topic(self, name):
        ic = self.communicator()
        mgr = 'IceStorm/TopicManager -t:tcp -h 127.0.0.1 -p 7910'
        mgr = ic.stringToProxy(mgr)
        mgr = IceStorm.TopicManagerPrx.checkedCast(mgr)

        try:
            return mgr.retrieve(name)
        except IceStorm.NoSuchTopic:
            return mgr.create(name)


if __name__ == '__main__':
    Publisher().main(sys.argv)

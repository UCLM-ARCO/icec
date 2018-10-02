#!/usr/bin/python3
# -*- mode: python3; coding: utf-8 -*-

import sys
import Ice
import time
import libcitisim as citisim
from libcitisim import SmartObject


class Client(Ice.Application):
    def run(self, args):
        if len(args) < 2:
            print("Usage: {} <proxy> [image]".format(args[0]))
            return -1

        ic = self.communicator()
        proxy = ic.stringToProxy(args[1])
        proxy = proxy.ice_encodingVersion(Ice.Encoding_1_0)
        screen = SmartObject.DataSinkPrx.uncheckedCast(proxy)

        image = "stop.tsv"
        if len(args) > 2:
            image = args[2]

        image = self.load_image(image)
        data = bytes(image)

        metadata = citisim.MetadataHelper(
            timestamp=time.time(),
            quality=100,
            expiration=0,
            latitude=0.0,
            longitude=0.0,
            altitude=0.0,
            place="model"
        ).to_dict()

        screen.notify(data, "model", metadata)
        print("image sent!")

    def load_image(self, filename):
        result = {}
        exec(open(filename).read(), result)
        return result['image']


if __name__ == "__main__":
    Client().main(sys.argv)

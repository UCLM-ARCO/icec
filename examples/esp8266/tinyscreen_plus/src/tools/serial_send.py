#!/usr/bin/python3 -u
# -*- mode: python; coding: utf-8 -*-

import sys
import serial
import time

class SerialSend:
    def __init__(self):
        filename = '/dev/ttyACM0'
        speed = 115200
        self.device = serial.Serial(filename, speed, timeout=0.1)

    def send_msg(self):
        print("--send message--")

        filename = "short.msg"
        if len(sys.argv) > 1:
            filename = sys.argv[1]

        msg = self.load(filename)
        print(" - size:", len(msg))
        self.device.write(msg)
        self.device.flush()

    def receive_data(self):
        print("--listening--")
        while True:
            data = self.device.read(1024)
            if not data:
                continue
            data = map(str, data.split(b"\n"))
            print("\n".join(data))

    def load(self, name):
        result = {}
        exec(open(name).read(), result)
        return result['msg']


if __name__ == "__main__":
    s = SerialSend()
    # print("--waiting init--")
    # time.sleep(5)

    s.send_msg()
    # s.receive_data()

# -*- mode: python; coding: utf-8 -*-

import unittest
from commodity.testing import wait_that
from commodity.net import localhost, listen_port
from commodity.os_ import SubProcess


class TestExampleWriterClient(unittest.TestCase):
    cwd = "examples/hello"

    def test_works(self):
        # run python server
        server = SubProcess("./writer_server.py", cwd=self.cwd)
        wait_that(localhost, listen_port(7890, proto="udp"))

        # get server proxy from output
        out = server.stdout.readline()
        str_proxy = out.split("ready: ")[1].strip()[1:-1]

        # run c client
        client = SubProcess("./writer_client '{0}'".format(str_proxy), cwd=self.cwd)
        self.assertEqual(client.wait(), 0)

        # check output
        server.terminate()
        out = server.stdout.readline()
        self.assertIn("'write' request, got: 1551", out)


class TestExampleReaderServer(unittest.TestCase):
    cwd = "examples/hello"

    def test_works(self):
        # run c server
        server = SubProcess("./reader_server", cwd=self.cwd)
        wait_that(localhost, listen_port(7891))

        # get server proxy from output
        out = server.stdout.readline()
        str_proxy = out.split("ready: ")[1].strip()[1:-1]

        # run python client
        client = SubProcess("./reader_client.py '{0}'".format(str_proxy), cwd=self.cwd)
        self.assertEqual(client.wait(), 0)

        # check output
        server.terminate()
        out = server.stdout.readline()
        self.assertIn("'read' request, return: 1441", out)

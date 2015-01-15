# -*- mode: python; coding: utf-8 -*-

import prego
import hamcrest


class TestMixin(object):
    def build_example(self):
        build = prego.Task("build example using make")
        build.command("make", cwd=self.cwd)
        return build

    def run_server(self, cmd, **kwargs):
        server = prego.Task("server", detach=True)
        server.command(cmd, cwd=self.cwd, **kwargs)
        self.server = server

    def run_client(self, cmd, expected):
        client = prego.Task("client")
        client.wait_that(self.server, prego.running())
        client.command("{0} '{1}'".format(cmd, self.proxy), cwd=self.cwd)
        client.wait_that(
            self.server.lastcmd.stdout.content,
            hamcrest.contains_string(expected))


class TestExampleHelloUDPDatagramClient(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_udp_datagram_client"
        self.proxy = "WriterServer -d -e 1.0:udp -h 127.0.0.1 -p 7890"

        self.build_example()
        self.run_server("./writer_server.py")
        self.run_client("./writer_client", "'write' request, got: 1551")


class TestExampleHelloUDPDatagramServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_udp_datagram_server"
        self.proxy = "WriterServer -d -e 1.0:udp -h 127.0.0.1 -p 7890"

        self.build_example()
        self.run_server("./writer_server", expected=-15)
        self.run_client("./writer_client.py", "'write' request, got: 1551")


class TestExampleHelloTCPTwowayServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_tcp_twoway_server"
        self.proxy = "ReaderServer -t -e 1.0:tcp -h 127.0.0.1 -p 7891"

        self.build_example()
        self.run_server("./reader_server", expected=-15)
        self.run_client("./reader_client.py", "'read' request, return: 1441")


class TestExampleHelloTCPOnewayClient(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_tcp_oneway_client"
        self.proxy = "WriterServer -o -e 1.0:tcp -h 127.0.0.1 -p 7893"

        self.build_example()
        self.run_server("./writer_server.py")
        self.run_client("./writer_client", "'write' request, got: 1551")


class TestExampleHelloTCPOnewayServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_tcp_oneway_server"
        self.proxy = "WriterServer -o -e 1.0:tcp -h 127.0.0.1 -p 7894"

        self.build_example()
        self.run_server("./writer_server", expected=-15)
        self.run_client("./writer_client.py", "'write' request, got: 1551")


class TestExampleSequences(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/sequences"
        self.proxy = 'VideoFrame -e 1.0 -d:udp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server", expected=-15)
        self.run_client("./client", "VideoFrame: showImage, size: 256")


class TestExampleSequencesMixedArgs(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/sequences_mixed_args"
        self.proxy = 'VideoFrame -e 1.0 -d:udp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server", expected=-15)
        self.run_client("./client", "VideoFrame: showImage, ts: 1, size: 256")

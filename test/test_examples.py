# -*- mode: python; coding: utf-8 -*-

import prego
import hamcrest
import uuid


class TestMixin(object):
    def build_example(self):
        build = prego.Task("build example using make")
        build.command("make", cwd=self.cwd)
        return build

    def run_server(self, cmd, **kwargs):
        params = {'expected': None, 'timeout': 10}
        params.update(kwargs)
        self.server = prego.Task("server", detach=True)
        self.server.command(cmd, cwd=self.cwd, **params)

    def run_client(self, cmd, expected, on_client=False, delayed=0):
        client = prego.Task("client")
        client.wait_that(self.server, prego.running(), delta=0.1)

        if delayed:
            client.delay(delayed)

        params = {'expected': None, 'timeout': 10}
        client.command("{0} '{1}'".format(cmd, self.proxy), cwd=self.cwd,
                       **params)
        stdout = self.server.lastcmd.stdout
        if on_client:
            stdout = client.lastcmd.stdout

        client.wait_that(
            stdout.content,
            hamcrest.contains_string(expected),
            delta=0.1)
        return client

    def run_client_alone(self, cmd, expected, delayed=0):
        client = prego.Task("client")
        if delayed:
            client.delay(delayed)

        params = {'expected': None, 'timeout': 10}
        client.command(
            "{0} '{1}'".format(cmd, self.proxy), cwd=self.cwd, **params)
        stdout = client.lastcmd.stdout
        client.wait_that(
            stdout.content,
            hamcrest.contains_string(expected),
            delta=0.1)
        return client


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
        self.run_server("./writer_server")
        self.run_client("./writer_client.py", "'write' request, got: 1551")


class TestExampleHelloTCPTwowayServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_tcp_twoway_server"
        self.proxy = "ReaderServer -t -e 1.0:tcp -h 127.0.0.1 -p 7891"

        self.build_example()
        self.run_server("./reader_server")
        self.run_client("./reader_client.py", "'read' request, return: 1441")


class TestExampleHelloTCPOnewayClient(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_tcp_oneway_client"
        self.proxy = "WriterServer -o -e 1.0:tcp -h 127.0.0.1 -p 7893"

        self.build_example()
        self.run_server("./server.py")
        self.run_client("./client_x86", "'write' request, got: 1551")


class TestExampleHelloTCPTwowayClient(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_tcp_twoway_client"
        self.proxy = "ReaderServer -e 1.0 -t:tcp -h 127.0.0.1 -p 7893"

        self.build_example()
        self.run_server("./server.py")
        self.run_client("./client_x86", "'read' request, returns 1999")


class TestExampleHelloTCPOnewayServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/hello_tcp_oneway_server"
        self.proxy = "WriterServer -o -e 1.0:tcp -h 127.0.0.1 -p 7894"

        self.build_example()
        self.run_server("./writer_server")
        self.run_client("./writer_client.py", "'write' request, got: 1551")


class TestExampleSequences(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/sequences"
        self.proxy = 'VideoFrame -e 1.0 -d:udp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server")
        self.run_client("./client", "VideoFrame: showImage, size: 256")


class TestExampleSequencesMixedArgs(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/sequences_mixed_args"
        self.proxy = 'VideoFrame -e 1.0 -d:udp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server")
        self.run_client("./client", "VideoFrame: showImage, size: 16x16, size: 256")


class TestExampleSequencesNested(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/sequences_nested"
        self.proxy = 'Vehicle -e 1.0 -d:udp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server")
        self.run_client("./client",
                        "Vehicle: sendCommand:\n"
                        " - id: 123\n"
                        " - opcode: 456\n"
                        " - params.length: 3\n"
                        " - p3.id = 23\n"
                        " - p3.data = 'a string'\n")


class TestExampleStringsClient(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/strings_client"
        self.proxy = "HelloServer -d -e 1.0:udp -h 127.0.0.1 -p 7896"

        self.build_example()
        self.run_server("./server.py")
        self.run_client("./client",
                        "'sayHelloFrom' request, from: 'Catherine Foo', times: 25")


class TestExampleStringsServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/strings_server"
        self.proxy = "HelloServer -d -e 1.0:udp -h 127.0.0.1 -p 7896"

        self.build_example()
        self.run_server("./server")
        self.run_client("./client.py",
                        "'sayHelloFrom' request, from: 'Jonh Doe', times: 5")


class TestExampleDictionaryClient(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/dictionary_client"
        self.proxy = "PropertyServer -d -e 1.0:udp -h 127.0.0.1 -p 7897"

        self.build_example()
        self.run_server("./server.py")
        self.run_client("./client",
                        "'update' request, {'name': 'sensor', 'type': 'humidity'}")


class TestExampleDictionaryServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/dictionary_server"
        self.proxy = "PropertyServer -d -e 1.0:udp -h 127.0.0.1 -p 7897"

        self.build_example()
        self.run_server("./server")

        client = prego.Task("client")
        client.wait_that(self.server, prego.running(), delta=0.1)
        client.command("{0} '{1}'".format("./client.py", self.proxy), cwd=self.cwd,
                       expected=None)

        client.wait_that(
            self.server.lastcmd.stdout.content,
            hamcrest.contains_string("'name': 'sensor'"),
            delta=0.1)

        client.wait_that(
            self.server.lastcmd.stdout.content,
            hamcrest.contains_string("'type': 'humidity'"),
            delta=0.1)


class TestExampleExtendedInterfaces(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/extended_interfaces"
        self.proxy = 'Server -e 1.0 -d:udp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server")
        self.run_client("./client", "hello\nbye")


class TestExampleDefaultServant(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/default_servant"
        oid = uuid.uuid1().hex[:6]
        self.proxy = "{} -e 1.0 -d:udp -h 127.0.0.1 -p 7900".format(oid)

        self.build_example()
        self.run_server("./server")
        self.run_client("./client.py", "'ping' request")


class TestExampleDynamicIceServer(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/dynamic_ice_server"
        self.proxy = "DynServer -e 1.0 -o:tcp -h 127.0.0.1 -p 7901"

        self.build_example()
        self.run_server("./server")
        self.run_client("./client.py", "'ice_invoke' called")


class TestExampleFloat(prego.TestCase, TestMixin):
    def test_server_works(self):
        self.cwd = "examples/float"
        self.proxy = 'Thermostat -e 1.0 -o:tcp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server")
        self.run_client("./client.py", "setTemperature to 25.6")

    def test_client_works(self):
        self.cwd = "examples/float"
        self.proxy = 'Thermostat -e 1.0 -o:tcp -h 127.0.0.1 -p 7892'

        self.build_example()
        self.run_server("./server.py")
        self.run_client("./client", "setTemperature to 25.6")


class TestExampleUseIcePrefix(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/slice2c_ice_prefix"

        build = prego.Task("build example using make")
        build.command("make", cwd=self.cwd)


class TestSendProxy(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/proxy_send"
        self.proxy = "Speaker -e 1.0 -t:tcp -h 127.0.0.1 -p 7902"

        self.build_example()
        self.run_server("./server.py")
        self.run_client("./client", "Speaker called properly")


class TestReceiveProxy(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/proxy_receive"
        self.proxy = "Speaker -e 1.0 -t:tcp -h 127.0.0.1 -p 7903"

        self.build_example()
        self.run_server("./server")
        self.run_client("./client.py", "Speaker called properly",
                        on_client=True)


class TestProxyAsReturnValue(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/proxy_as_retval"
        self.proxy = "Manager -e 1.0 -t:tcp -h 127.0.0.1 -p 7904"

        self.build_example()
        self.run_server("./server.py")
        self.run_client("./client", "Speaker called properly")


class TestStringToProxy(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/communicator_stringToProxy"
        self.proxy = "Manager -e 1.0 -t:tcp -h 127.0.0.1 -p 7904"

        self.build_example()
        self.run_client_alone(
            "./test", "Proxy: 'Object1 -t:tcp -h 127.0.0.1 -p 1234'")


class TestIceStormPublish(prego.TestCase, TestMixin):
    def test_works(self):
        self.cwd = "examples/icestorm_lite_publish"
        self.build_example()

        self.proxy = "IceStorm/TopicManager -e 1.0 -t:tcp -h 127.0.0.1 -p 7910"
        self.run_server('make run-is')
        self.run_server('./subscriber.py')
        self.run_client("./publisher", "Hello from IceStorm!", delayed=0.5)

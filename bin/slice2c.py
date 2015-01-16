#!/usr/bin/python -u
# -*- mode: python; coding: utf-8 -*-

import os
import sliceparser
from commodity.pattern import Bunch


class CCode(list):
    def __init__(self):
        self.tabcount = 0

    def __iadd__(self, item):
        self.append(item)
        return self

    def __add__(self, item):
        self.append(item)
        return self

    def __str__(self):
        retval = ""
        for i in self:
            retval += str(i)
        return retval

    def nl(self, count=1):
        self.append("\n" * count)
        return self

    def ret(self, value=None):
        retval = ""
        if value is not None:
            retval = " {0}".format(value)
        self.append("return{0};\n".format(retval))

    def comment(self, content):
        self += Comment(content)
        return self

    def get_tab(self):
        return " " * self.tabcount * 4

    def set_tab(self, value):
        self.tabcount = value

    tab = property(get_tab, set_tab)

    def tabify(self, code):
        retval = self.tab + code.replace("\n", "\n" + self.tab)
        if retval.endswith("\n" + self.tab):
            retval = retval[:-len(self.tab)]
        return retval


class Comment(object):
    def __init__(self, content, start="/*", end="*/"):
        self.start = start
        self.end = end
        self.content = content

    def __str__(self):
        return "{0}{1}{2}\n".format(self.start, self.content, self.end)


class Assert(object):
    def __init__(self, assertion):
        self.assertion = assertion

    def __str__(self):
        return "assert({0});\n".format(self.assertion)


class ThrowException(object):
    def __init__(self, name, *params):
        self.name = name
        self.params = params

    def __str__(self):
        return 'throw("{0}", {1});\n'.format(self.name, ", ".join(self.params))


class Casting(object):
    def __init__(self, typeid, target):
        self.typeid = typeid
        self.target = target

    def __str__(self):
        return "({0}){1}".format(self.typeid, str(self.target))


class Include(object):
    def __init__(self, filename=None, local=False):
        self._filenames = []
        if filename is not None:
            self._filenames.append((filename, local))

    def add_local(self, filename):
        self._filenames.append((filename, True))
        return self

    def add_system(self, filename):
        self._filenames.append((filename, False))
        return self

    def __repr__(self):
        return str(self)

    def __str__(self):
        retval = "\n"
        for f, local in self._filenames:
            s = "#include " + ('"{0}"' if local else '<{0}>')
            retval += s.format(f) + "\n"
        return retval


class Enum(CCode):
    def __str__(self):
        retval = "enum {\n"

        tab = " " * 4
        for i in self:
            retval += tab + str(i) + ",\n"

        retval = retval[:-2] + "\n"
        retval += "}"
        return retval


class Typedef(object):
    def __init__(self, name, body):
        self.name = name
        self.body = body

    def __str__(self):
        return "typedef {0} {1};\n".format(self.body, self.name)


class Struct(CCode):
    def __str__(self):
        retval = "struct {\n"
        tab = " " * 4
        for i in self:
            retval += "{0}{1};\n".format(tab, i)
        retval += "}"
        return retval


class Param(object):
    def __init__(self, typeid, name, kind=None):
        self.typeid = typeid
        self.name = name
        self.kind = kind
        self.is_proxy = False

    def declare(self):
        retval = ""
        typeid = self.typeid
        if self.is_proxy:
            typeid += "Prx"
        retval += "{0} {1}".format(typeid, self.name)

        tid = self.typeid
        if tid.startswith("Ice_"):
            tid = tid[len("Ice_"):].lower()

        if self.kind == "enum" or tid in ["bool", "char", "byte", "int", "short"]:
            retval += " = 0"

        retval += ";\n"
        return retval

    def readFromStream(self, ins_name, opargs=None, as_size=False):
        ins = InputStream(ins_name)
        if as_size:
            ins.readSize("&" + self.name, *opargs)
        else:
            ins.read(self.typeid, "&" + self.name)
        return ins

    def __str__(self):
        return "{0} {1}".format(self.typeid, self.name)


class ParamArray(object):
    def __init__(self, typeid, count, name):
        self.typeid = typeid
        self.name = name
        self.count = count

    def declare(self):
        return "{0} {1}[{2}];\n".format(self.typeid, self.name, self.count)

    def readFromStream(self, ins_name):
        ins = InputStream(ins_name)
        ins.read(self.typeid + "*", self.name)
        return ins


class Pointer(object):
    def __init__(self, type, use_ptr=False):
        self.type = type
        self.use_ptr = use_ptr

    def __str__(self):
        ptr = "*"
        if self.use_ptr:
            ptr = "Ptr"
        return "{0}{1}".format(self.type, ptr)


class Function(CCode):
    def __init__(self, declaration=False):
        CCode.__init__(self)
        self._declaration = declaration
        self._static = False
        self._weak = False
        self._retval = "void"
        self._name = "function"
        self._params = []
        self._vars = []

        self += FunctionCall("trace")
        self += "\n"

    def static(self):
        self._static = True
        return self

    def weak(self):
        self._weak = True
        return self

    def retval(self, typeid):
        self._retval = typeid
        return self

    def void(self):
        return self.retval("void")

    def name(self, name):
        self._name = name
        return self

    def param(self, param):
        self._params.append(param)
        return self

    def declare_var(self, var):
        self._vars.append(var)
        return self

    def __str__(self):
        retval = ""
        if self._static:
            retval += "static "

        retval += str(self._retval)
        retval += "\n"
        retval += self._name + "("

        tab = " " * (len(self._name) + 1)
        for i, p in enumerate(self._params):
            if i > 0:
                retval += tab
            retval += str(p) + ",\n"
        if self._params:
            retval = retval[:-2] + ")"

        if self._declaration:
            if self._weak:
                retval += " __attribute__((weak))"

            return retval + ";\n"

        retval += " {\n"

        self.tab = 1
        for i in self._vars:
            retval += self.tabify(str(i.declare()))

        retval += "\n"
        for i in self:
            retval += self.tabify(str(i).strip(" "))

        retval += "}\n"
        return retval


class FunctionCall(object):
    def __init__(self, name, *args):
        self._name = name
        self._args = args
        self._result = None

    def result(self, name="result"):
        self._result = name

    def __str__(self):
        args = ""
        for i in self._args:
            args += "{0}, ".format(i)
        args = args[:-2]

        retval = ""
        if self._result is not None:
            retval = "{0} = ".format(self._result)

        retval += "{0}({1});\n".format(self._name, args)

        if len(retval) > 80:
            tab = " " * 8
            retval = retval.replace(", ", ",\n{0}".format(tab))
        return retval


def Ptr_check(param):
    return FunctionCall("Ptr_check", param.name)


class OutputStream(CCode):
    def __init__(self, this, initializer=None):
        CCode.__init__(self)
        self.initializer = initializer
        self.this = this

    def declare(self):
        retval = "Ice_OutputStream"
        if self.initializer is not None:
            retval += "Ptr"
        retval += " " + str(self.this)
        if self.initializer is not None:
            retval += " = {0}".format(self.initializer)
        retval += ";\n"
        return retval

    def init(self):
        name = "Ice_OutputStream_init"
        self.append(FunctionCall(name, self.this))

    def writeSize(self, param):
        name = "Ice_OutputStream_writeSize"
        self.append(FunctionCall(name, self.this, param))

    def writeBlob(self, data, size):
        name = "Ice_OutputStream_writeBlob"
        self.append(FunctionCall(name, self.this, data, size))

    def writeString(self, data):
        name = "Ice_OutputStream_writeString"
        self.append(FunctionCall(name, self.this, '"{0}"'.format(data)))

    def write(self, typeid, param, kind=None, toPointer=True):
        params = []
        name = ""

        if typeid in ["bool", "char", "byte", "short", "int"]:
            if toPointer:
                param = "&({0})".format(param)
            name = "Ice_OutputStream_write{0}".format(typeid.capitalize())

        if typeid in ["Ice_Identity"]:
            name = "Ice_OutputStream_writeIdentity"

        if kind in ["enum"]:
            name = "Ice_OutputStream_writeByte"

        if kind in ["proxy"]:
            name = "Ice_ObjectPrx_writeToOutputStream"
            if toPointer:
                param = "&({0})".format(param)
            params = [param, self.this]

        if name and not params:
            params = [self.this, param]

        if not params:
            param = "&({0})".format(param)
            name = typeid + "_writeToOutputStream"
            params = [param, self.this]

        self.append(FunctionCall(name, *params))

    def startWriteEncaps(self):
        name = "Ice_OutputStream_startWriteEncaps"
        self.append(FunctionCall(name, self.this))

    def endWriteEncaps(self):
        name = "Ice_OutputStream_endWriteEncaps"
        self.append(FunctionCall(name, self.this))

    def setMessageSize(self):
        name = "Ice_OutputStream_setMessageSize"
        self.append(FunctionCall(name, self.this))


class InputStream(CCode):
    def __init__(self, this):
        self.this = this

    def readSize(self, *params):
        name = "Ice_InputStream_readSize"
        self.append(FunctionCall(name, self.this, *params))

    def read(self, typeid, param, kind=None, toPointer=False):
        params = []
        name = ""

        if toPointer:
            param = "&({0})".format(param)

        if typeid in ["bool", "char", "byte", "short", "int"]:
            name = "Ice_InputStream_read{0}".format(typeid.capitalize())

        if typeid == "char*":
            name = "Ice_InputStream_readString"

        if kind in ["enum"]:
            name = "Ice_InputStream_readByte"
            param = "(Ice_Byte*){0}".format(param)

        if kind in ["proxy"]:
            name = "Ice_ObjectPrx_readFromInputStream"
            params = [param, self.this]

        if name and not params:
            params = [self.this, param]

        if not params:
            name = typeid + "_readFromInputStream"
            params = [param, self.this]

        self.append(FunctionCall(name, *params))

    def alloca(self, typeid, dest, size):
        sizeof = "sizeof({0}) * {1}".format(str(typeid), size)
        alloca = FunctionCall("alloca", sizeof)
        alloca.result(dest)
        self.append(alloca)


class ForLoop(CCode):
    def __init__(self, varname, initial, condition, increment):
        self.varname = varname
        self.counter = Param("int", varname)

        self.initial = initial
        self.condition = condition
        self.increment = increment

    def __str__(self):
        retval = "for ({0}={1}; {2}; {3}) {{\n".format(
            self.varname, self.initial, self.condition, self.increment)

        self.tab = 1
        for i in self:
            retval += self.tabify(str(i))

        retval += "}\n"
        return retval


class IfCondition(CCode):
    def __init__(self, condition):
        self.condition = condition

    def __str__(self):
        retval = "if ({0}) {{\n".format(self.condition)

        self.tab = 1
        for i in self:
            retval += self.tabify(str(i))

        retval += "}\n"
        return retval


class IceCBuilder(object):
    def __init__(self, args):
        self.args = args

    def add_coding_comment(self, c):
        c += Comment(" -*- mode: c; coding: utf-8 -*- ")

    def add_common_includes(self, c):
        includes = Include()
        includes.add_local("IceC/IceC.h")
        includes.add_system("alloca.h")
        c += includes
        c.nl()

    def get_output_filename(self):
        name = os.path.splitext(self.args.input)[0]
        return name + ".h"

    def build(self, st):
        output = self.get_output_filename()
        if os.path.exists(output):
            print "File '{0}' exists, nothing done".format(output)
            return

        with file(output, "w") as dst:
            code = self.build_all(st)
            dst.write(str(code))

    def build_all(self, st):
        self.st = st
        c = CCode()

        self.add_coding_comment(c)
        self.add_common_includes(c)

        for i in self.st.items:
            i.render(c)

        return c


class BaseDecl(object):
    def __init__(self, module, ptr):
        self.module = module
        self.name = ptr.name()
        self.full_name = module.name + "_" + self.name

    def cpp2c_name(self, name):
        if name.startswith("::"):
            name = name[2:]
        return name.replace("::", "_")

    def render(self, c):
        c += "Not implemented"
        c.nl()

    def render_ptr(self, c):
        nameptr = self.full_name + "Ptr"
        c += Typedef(nameptr, Pointer(self.full_name))
        c.nl()

    def render_marshaller(self, c):
        c += self.get_marshall_func()
        c.nl()

    def render_children(self, c):
        for i in self:
            i.render(c)

    def get_marshall_func(self):
        name = self.full_name + "_writeToOutputStream"
        this = Param(Pointer(self.full_name, True), "this")
        outs = Param("Ice_OutputStreamPtr", "os")
        func = (Function()
                .static().void()
                .name(name)
                .param(this)
                .param(outs))

        func += Ptr_check(outs)
        func.nl()

        return func

    def get_unmarshall_func(self):
        name = self.full_name + "_readFromInputStream"
        this = Param(Pointer(self.full_name, True), "this")
        ins = Param("Ice_InputStreamPtr", "is")
        func = (Function()
                .static().void()
                .name(name)
                .param(this)
                .param(ins))

        func += Ptr_check(ins)
        func.nl()

        return func


class ModuleDecl(BaseDecl):
    def __init__(self, ptr):
        self.name = ptr.name()


class ClassDefForward(BaseDecl):
    def render(self, c):
        c.comment(" forward declaration ")

        # empty struct
        struct_name = "struct " + self.full_name
        c += struct_name + ";\n"

        # typedef for pointer to struct
        nameptr = self.full_name + "Ptr"
        c += Typedef(nameptr, Pointer(struct_name))

        # typedef for proxy
        proxy = self.full_name + "Prx"
        c += Typedef(proxy, "Ice_ObjectPrx")

        # typedef for pointer to proxy
        proxyptr = self.full_name + "PrxPtr"
        c += Typedef(proxyptr, "Ice_ObjectPrxPtr")
        c.nl()


class ClassDefDecl(BaseDecl, list):
    def __init__(self, module, ptr, has_forward=False):
        BaseDecl.__init__(self, module, ptr)
        self.has_forward = has_forward

    def render(self, c):
        self.render_struct(c)

        if not self.has_forward:
            self.render_ptr(c)

        self.render_proxy_defs(c)
        self.render_forwards(c)
        self.render_init(c)
        self.render_servant_methods(c)
        self.render_servant_dispatchers(c)
        self.render_servant_handler(c)
        self.render_proxy_methods(c)

    def render_struct(self, c):
        st = Struct()

        base = Param("Ice_Object", "__base")
        st.append(base)

        c += Typedef(self.full_name, st)
        c.nl()

    def render_proxy_defs(self, c):
        c += Typedef(self.full_name + "Prx", "Ice_ObjectPrx")
        c += Typedef(self.full_name + "PrxPtr", Pointer("Ice_ObjectPrx"))
        c.nl()

    def render_forwards(self, c):
        name = self.full_name + "_methodHandler"
        this = Param(Pointer(self.full_name, True), "this")
        ins = Param("Ice_InputStreamPtr", "is")
        outs = Param("Ice_OutputStreamPtr", "os")
        func = (Function(True)
                .static().void()
                .name(name)
                .param(this)
                .param(ins)
                .param(outs))

        c.comment(" forward declaration ")
        c += func
        c.nl()

    def render_init(self, c):
        name = self.full_name + "_init"
        this = Param(Pointer(self.full_name, True), "this")
        func = (Function()
                .static().void()
                .name(name)
                .param(this))

        handler = self.full_name + "_methodHandler"
        func += FunctionCall("Ice_Object_init",
                             Casting("Ice_ObjectPtr", "this"),
                             Casting("Ice_MethodHandlerPtr", handler))

        c += func
        c.nl()

    def render_servant_methods(self, c):
        for i in self:
            i.render_as_servant_method(c)

    def render_servant_dispatchers(self, c):
        for i in self:
            i.render_as_dispatcher(c)

    def render_servant_handler(self, c):
        name = self.full_name + "_methodHandler"
        this = Param(Pointer(self.full_name, True), "this")
        ins = Param("Ice_InputStreamPtr", "is")
        outs = Param("Ice_OutputStreamPtr", "os")
        func = (Function()
                .static().void()
                .name(name)
                .param(this)
                .param(ins)
                .param(outs))

        func += Ptr_check(this)
        func += Ptr_check(ins)
        func += Ptr_check(outs)
        func.nl()

        operationSize = Param("int", "operationSize")
        func.declare_var(operationSize)
        func += operationSize.readFromStream("is", opargs=["false"], as_size=True)
        func += Assert("operationSize <= MAX_OPERATION_NAME_SIZE")
        func.nl()

        operation = ParamArray("char", "MAX_OPERATION_NAME_SIZE", "operation")
        func.declare_var(operation)
        func += operation.readFromStream("is")
        func.nl()

        func += Comment(" FIXME: operationMode ignored ")
        mode = Param("byte", "mode")
        func.declare_var(mode)
        func += mode.readFromStream("is")
        func.nl()

        func += Comment(" FIXME: Context not supported ")
        contextSize = Param("byte", "contextSize")
        func.declare_var(contextSize)
        func += contextSize.readFromStream("is")
        func += Assert("contextSize == 0")
        func.nl()

        encapSize = Param("int", "encapSize")
        func.declare_var(encapSize)
        func += encapSize.readFromStream("is")

        major = Param("byte", "major")
        minor = Param("byte", "minor")
        func.declare_var(major)
        func.declare_var(minor)
        func += major.readFromStream("is")
        func += minor.readFromStream("is")

        func += Assert("major == 1 && minor == 0")
        func += Assert("encapSize - 6 == (is->size - (is->next - is->data))")
        func.nl()

        for i in self:
            cond = IfCondition('strcmp(operation, "{0}") == 0'.format(i.name))
            cond += FunctionCall(i.full_name + "__dispatch", "this", "is", "os")
            cond.ret()
            func += cond
            func.nl()

        func += ThrowException("Ice_OperationNotExistException", "__FILE__", "__LINE__")
        c += func
        c.nl()

    def render_proxy_methods(self, c):
        for i in self:
            i.render_as_proxy_method(c)


class ParamDecl(BaseDecl):
    def __init__(self, st, ptr):
        self.name = ptr.name()
        self.type = TypeDecl(ptr.type())

        self.contained_type = None
        if self.type.kind == "sequence":
            self.contained_type = TypeDecl(ptr.type().type())

        self.is_proxy = st.is_interface(self.cpp2c_name(self.type.typeid))

    def __str__(self):
        prx = ""
        if self.is_proxy:
            prx = "PrxPtr"
        return "{0}{1} {2}".format(str(self.type), prx, self.name)

    def declare(self):
        retval = CCode()

        typeid = str(self.type)
        if self.is_proxy:
            typeid += "Prx"

        param = Param(typeid, self.name, kind=self.type.kind)
        retval += param.declare()
        return retval


class OperationDecl(BaseDecl):
    def __init__(self, st, ptr):
        iface = st.top
        self.name = ptr.name()
        self.iface_name = iface.full_name
        self.full_name = self.iface_name + "_" + self.name
        self.retval = TypeDecl(ptr.returnType())

        self.params = []
        for p in ptr.parameters():
            self.params.append(ParamDecl(st, p))

    def render_as_servant_method(self, c):
        name = self.iface_name + "I_" + self.name
        this = Param(Pointer(self.iface_name, True), "this")

        func = (Function(True)
                .retval(self.retval)
                .name(name)
                .param(this)
                .weak())

        for p in self.params:
            func.param(p)

        c += func
        c.nl()

    def render_as_dispatcher(self, c):
        name = self.iface_name + "_" + self.name + "__dispatch"
        this = Param(Pointer(self.iface_name, True), "this")
        ins = Param(Pointer("Ice_InputStream", True), "is")
        outs = Param(Pointer("Ice_OutputStream", True), "os")

        func = (Function()
                .static()
                .name(name)
                .param(this)
                .param(ins)
                .param(outs))

        # unmarshall params
        call_args = ["this"]
        for i in self.params:
            func.declare_var(i)

            ins = InputStream("is")
            kind = "proxy" if i.is_proxy else i.type.kind

            # if sequence, reserve space to hold it
            if kind == "sequence":
                func.comment(" allocate space for sequence ")
                size_var = i.name + ".size"
                items_var = i.name + ".items"

                ins.readSize("&" + size_var, "false")
                ins.alloca(self.cpp2c_name(i.type.typeid), items_var, size_var)

            ins.read(self.cpp2c_name(i.type.typeid), "&" + i.name, kind)
            func += ins
            func.nl()

            arg = i.name
            if i.is_proxy:
                arg = "&" + arg
            call_args.append(arg)

        # call to servant method
        name = self.iface_name + "I_" + self.name
        method = FunctionCall(name, *call_args)
        if self.retval.typeid != "void":
            method.result("_retval")
            func.declare_var(Param(self.retval.typeid, "_retval"))

        func += method

        # write result on output stream
        if self.retval.typeid != "void":
            outs = OutputStream("os")
            outs.write(self.cpp2c_name(self.retval.typeid), "_retval", toPointer=False)
            func += outs

        c += func
        c.nl()

    def render_as_proxy_method(self, c):
        name = self.iface_name + "_" + self.name
        this = Param(Pointer(self.iface_name + "Prx", True), "this")

        func = (Function()
                .static()
                .retval(self.retval)
                .name(name)
                .param(this))

        for p in self.params:
            func.param(p)

        func += Ptr_check(this)
        func.nl()

        func += FunctionCall("Ice_ObjectPrx_connect", "this")
        outs = OutputStream("os", "&(this->stream)")
        outs.init()

        outs.nl().comment(" request header ")
        outs.writeBlob("Ice_header", "sizeof(Ice_header)")

        outs.nl().comment(" request body ")
        outs.write("int", 0, toPointer=False)              # request id
        outs.write("Ice_Identity", "this->identity")       # object identity
        outs.writeString("")                               # facet, only empty allowed
        outs.writeString(self.name)                        # operation
        outs.write("byte", "Ice_Normal", toPointer=False)  # operation mode
        outs.write("byte", 0, toPointer=False)             # context, not supported

        outs.nl().comment(" encapsulated params ")

        outs.startWriteEncaps()

        for p in self.params:
            kind = "proxy" if p.is_proxy else p.type.kind
            outs.write(self.cpp2c_name(p.type.typeid), p.name, kind=kind, toPointer=False)
        outs.endWriteEncaps()
        outs.setMessageSize()

        func.declare_var(outs)
        func += outs
        func.nl()

        func += FunctionCall("Ice_ObjectPrx_send", "this")
        func.nl().comment(" FIXME: wait response and return correct value ")

        c += func
        c.nl()


class TypeDecl(BaseDecl):
    def __init__(self, ptr):
        self.typeid = "void"
        self.kind = None

        if ptr is not None:
            self.typeid = ptr.typeId()
            self.kind = str(ptr.__class__.__name__).lower()

    def __repr__(self):
        if self.typeid in ["bool", "byte", "int", "short"]:
            return "Ice_" + self.typeid.capitalize()

        return self.cpp2c_name(self.typeid)


class SequenceDecl(BaseDecl):
    def __init__(self, module, ptr):
        BaseDecl.__init__(self, module, ptr)
        self.type = TypeDecl(ptr.type())

    def render(self, c):
        self.render_sequence(c)
        self.render_ptr(c)
        self.render_marshaller(c)
        self.render_unmarshaller(c)

    def render_sequence(self, c):
        seq = Struct()

        items = Param(Pointer(self.type), "items")
        seq.append(items)
        size = Param("int", "size")
        seq.append(size)

        c += Typedef(self.full_name, seq)
        c.nl()

    def render_marshaller(self, c):
        fn = self.get_marshall_func()

        # write size of sequence
        outs1 = OutputStream("os")
        outs1.writeSize("this->size")
        fn += outs1
        fn.nl()

        # write items of sequence
        outs2 = OutputStream("os")
        outs2.write(self.cpp2c_name(self.type.typeid),
                    "*(this->items + i)",
                    toPointer=False)

        loop = ForLoop("i", "0", "i<this->size", "i++")
        loop += outs2
        fn += loop
        fn.declare_var(loop.counter)

        c += fn
        c.nl()

    def render_unmarshaller(self, c):
        fn = self.get_unmarshall_func()

        # read size of sequence
        ins1 = InputStream("is")
        ins1.readSize("&(this->size)", "true")
        fn += ins1
        fn.nl()

        # zero-copy optimization, for byte only
        if self.type.typeid == "byte":
            fn.comment(" zero-copy optimization ")
            fn += "this->items = is->next;\n"
            fn += "is->next += this->size;\n"

        # read items of sequence
        else:
            ins2 = InputStream("is")
            ins2.read(self.cpp2c_name(self.type.typeid), "this->items + i")

            loop = ForLoop("i", "0", "i<this->size", "i++")
            loop += ins2
            fn += loop
            fn.declare_var(loop.counter)

        c += fn
        c.nl()


class StructDecl(BaseDecl):
    def __init__(self, module, ptr):
        BaseDecl.__init__(self, module, ptr)

        self.members = []
        for i in ptr.dataMembers():
            type = i.type()
            member = Bunch(typeid = type.typeId(),
                           name = i.name(),
                           kind = str(type.__class__.__name__).lower())
            self.members.append(member)

    def render(self, c):
        self.render_struct(c)
        self.render_ptr(c)
        self.render_marshaller(c)
        self.render_unmarshaller(c)

    def render_struct(self, c):
        struct = Struct()
        for m in self.members:
            p = Param(self.cpp2c_name(m.typeid), m.name)
            struct.append(p)

        c += Typedef(self.full_name, struct)
        c.nl()

    def render_marshaller(self, c):
        fn = self.get_marshall_func()

        outs = OutputStream("os")
        for m in self.members:
            param = "this->{0}".format(m.name)
            outs.write(self.cpp2c_name(m.typeid), param, m.kind, toPointer=False)
        fn += outs

        c += fn
        c.nl()

    def render_unmarshaller(self, c):
        fn = self.get_unmarshall_func()

        ins = InputStream("is")
        for m in self.members:
            param = "this->{0}".format(m.name)

            if m.kind == "enum" or m.typeid in ["bool", "char", "byte", "int", "short"]:
                fn += "{0} = 0;\n".format(param)

            ins.read(self.cpp2c_name(m.typeid), param, m.kind, toPointer=True)

        fn.nl()
        fn.comment(" FIXME: assure this->data is reserved!! ")
        fn += ins

        c += fn
        c.nl()


class EnumDecl(BaseDecl):
    def __init__(self, module, ptr):
        BaseDecl.__init__(self, module, ptr)
        self.values = [x.name() for x in ptr.getEnumerators()]

    def render(self, c):
        enum = Enum()
        for v in self.values:
            enum.append(self.name + "_" + v)

        c += Typedef(self.full_name, enum)
        c.nl()


class IceVisitor(sliceparser.Visitor):
    def __init__(self):
        self.items = []
        self.ifaces = []
        self.declared = []

    def __iter__(self):
        return iter(self.items)

    @property
    def top(self):
        return self.items[-1]

    def is_interface(self, typeid):
        for i in self.ifaces:
            if i.full_name == typeid:
                return True
        return False

    def visitModuleStart(self, moduleptr):
        self.module = ModuleDecl(moduleptr)
        return True

    def visitClassDecl(self, classdeclptr):
        self.ifaces.append(ClassDefDecl(self.module, classdeclptr))

        name = classdeclptr.name()
        if name not in self.declared:
            self.declared.append(name)
            self.items.append(ClassDefForward(self.module, classdeclptr))

    def visitClassDefStart(self, classdefptr):
        name = classdefptr.name()
        has_forward = name in self.declared

        cls = ClassDefDecl(self.module, classdefptr, has_forward)
        self.items.append(cls)
        self.ifaces.append(cls)

        if name not in self.declared:
            self.declared.append(name)

        return True

    def visitStructStart(self, structptr):
        self.items.append(StructDecl(self.module, structptr))
        return True

    def visitOperation(self, operationptr):
        self.top.append(OperationDecl(self, operationptr))

    def visitSequence(self, sequenceptr):
        self.items.append(SequenceDecl(self.module, sequenceptr))

    def visitEnum(self, enumptr):
        self.items.append(EnumDecl(self.module, enumptr))


if __name__ == "__main__":
    import argparse

    args = argparse.ArgumentParser()
    args.add_argument('input', help='Slice file')
    args = args.parse_args()

    parser = sliceparser.Parser(args.input)
    visitor = IceVisitor()
    builder = IceCBuilder(args)

    parser.accept(visitor)
    builder.build(visitor)


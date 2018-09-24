#!/usr/bin/python -u
# -*- mode: python; coding: utf-8 -*-

import os
import sliceparser
from commodity.pattern import Bunch


basic_types = ['bool', 'byte', 'short', 'int', 'long', 'float', 'double']
extended_types = basic_types + ['string']


def get_basic_type(src):
    return {
        'Ice_Bool': 'bool',
        'Ice_Byte': 'byte',
        'Ice_Short': 'short',
        'Ice_Int': 'int',
        'Ice_Long': 'long',
        'Ice_Float': 'float',
        'Ice_Double': 'double',
        'Ice_String': 'string',
    }.get(src, src)


def get_ice_type(src):
    return {
        'bool': 'Ice_Bool',
        'byte': 'Ice_Byte',
        'short': 'Ice_Short',
        'int': 'Ice_Int',
        'long': 'Ice_Long',
        'float': 'Ice_Float',
        'double': 'Ice_Double',
        'string': 'Ice_String'
    }.get(src, src)


class CCode(list):
    def __init__(self):
        self.nested = None
        self.tabcount = 0

    def __iadd__(self, item):
        self.append(item)
        return self

    def __add__(self, item):
        self.append(item)
        return self

    def __repr__(self):
        return str(self)

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
        if isinstance(value, Param):
            value = value.name
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

    def condition(self, condition):
        c = getattr(self, 'nested', None) or self

        class Context:
            def __enter__(self):
                c.nested = IfCondition(condition)

            def __exit__(self, type, value, traceback):
                nested = c.nested
                c.nested = None
                c.append(nested)

        return Context()

    def switch(self, arg):
        c = getattr(self, 'nested', None) or self

        class Context:
            def __enter__(self):
                c.nested = Switch(arg)
                return c.nested

            def __exit__(self, type, value, traceback):
                nested = c.nested
                c.nested = None
                c.append(nested)

        return Context()

    def append(self, item):
        if not hasattr(self, "nested") or self.nested is None:
            return list.append(self, item)
        return self.nested.append(item)


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
        return 'c_throw("{0}", {1});\n'.format(self.name, ", ".join(self.params))


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


class Alias(object):
    def __init__(self, src, dst):
        self.src = src
        self.dst = dst

    def __repr__(self):
        return str(self)

    def __str__(self):
        retval = "#define {} {}\n".format(self.src, self.dst)
        return retval


class Macro(CCode):
    def __init__(self, name, *args):
        CCode.__init__(self)
        self.name = name
        self.args = args

    def __str__(self):
        args = ", ".join(self.args)
        retval = "#define {}({})\n".format(self.name, args)
        retval += "do {\n"
        retval += CCode.__str__(self)
        retval += "} while(0)\n"
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
    def __init__(self, name=""):
        self.name = name

    def __str__(self):
        name = "{0} ".format(self.name)
        retval = "struct {0}{{\n".format(name)
        tab = " " * 4
        for i in self:
            retval += "{0}{1};\n".format(tab, i)
        retval += "}"
        return retval


# FIXME: mix this with ParamDecl
class Param(object):
    def __init__(self, typeid, name, kind=None, scope=None, local=False, value=None):
        self.typeid = typeid
        self.name = name
        self.kind = kind
        self.value = value

        if scope is not None:
            if local:
                scope += self.declare()
            else:
                scope.declare_var(self)

    def declare(self):
        retval = ""
        typeid = self.typeid
        retval += "{0} {1}".format(typeid, self.name)

        tid = self.typeid
        if tid.startswith("Ice_"):
            tid = tid[len("Ice_"):].lower()

        if self.value is not None:
            retval += " = {}".format(self.value)
        elif self.kind == "enum" or tid in basic_types:
            retval += " = 0"

        retval += ";\n"
        return retval

    def readFromStream(self, ins, opargs=None, as_size=False, is_pointer=False):
        if isinstance(ins, Param):
            ins = InputStream(ins.name)
        elif isinstance(ins, str):
            ins = InputStream(ins)

        assert(isinstance(ins, InputStream))
        if as_size:
            ins.readSize("&" + self.name, *opargs)
        else:
            name = ('&' if not is_pointer else '') + self.name
            ins.read(self.typeid, name, self.kind)
        return ins

    def ref(self):
        return "&" + self.name

    def __str__(self):
        return "{0} {1}".format(self.typeid, self.name)


class ParamArray(object):
    def __init__(self, typeid, count, name, initial_values=None, scope=None, local=False):
        self.typeid = typeid
        self.name = name
        self.count = count
        self.initial_values = initial_values

        if scope is not None:
            if local:
                scope += self.declare()
            else:
                scope.declare_var(self)

    def declare(self):
        dcl = "{0} {1}[{2}]".format(self.typeid, self.name, self.count)
        if self.initial_values:
            dcl += " = {"
            for i in self.initial_values:
                dcl += i + ", "
            dcl = dcl[:-2] + "}"
        dcl += ";\n"
        return dcl

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
        self._casting_to = None

    def result(self, name="result", casting_to=None):
        self._result = name
        self._casting_to = casting_to

    def __str__(self):
        args = ""
        for i in self._args:
            args += "{0}, ".format(i)
        args = args[:-2]

        retval = ""
        if self._result is not None:
            retval = "{0} = ".format(self._result)

        if self._casting_to is not None:
            retval += "({}) ".format(self._casting_to)

        retval += "{0}({1});\n".format(self._name, args)

        if len(retval) > 80:
            tab = " " * 8
            retval = retval.replace(", ", ",\n{0}".format(tab))
        return retval


def Ptr_check(param):
    return FunctionCall("Ptr_check", param.name)


class OutputStream(CCode):
    def __init__(self, this, initializer=None, scope=None):
        CCode.__init__(self)
        self.initializer = initializer
        self.this = this

        if scope is not None:
            scope.declare_var(self)

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

        typeid = get_basic_type(typeid)
        if typeid in extended_types:
            if toPointer:
                param = "&({0})".format(param)
            name = "Ice_OutputStream_write{0}".format(typeid.capitalize())

        if typeid == "string":
            name = "Ice_OutputStream_writeIceString"

        if typeid in ["Ice_Identity"]:
            if kind == "struct":
                param = "(IceStd_IdentityPtr)&({0})".format(param)
                name = "Ice_OutputStream_writeIceIdentity"
            else:
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
    def __init__(self, this, initializer=None, is_pointer=True, scope=None, local=False):
        self.this = this
        self.initializer = initializer
        self.is_pointer = is_pointer or initializer is not None
        self.holder = scope or self

        if scope is not None:
            if local:
                scope += self.declare()
            else:
                scope.declare_var(self)

    def declare(self):
        retval = "Ice_InputStream"
        if self.initializer is not None:
            retval += "Ptr"
        retval += " " + str(self.this)
        if self.initializer is not None:
            retval += " = {0}".format(self.initializer)
        retval += ";\n"
        return retval

    def init(self, source, type):
        name = "Ice_InputStream_init"
        self.holder += FunctionCall(name, self.ref(), source, type)

    def readSize(self, *params):
        name = "Ice_InputStream_readSize"
        self.holder += FunctionCall(name, self.ref(), *params)

    def read(self, typeid, param, kind=None, toPointer=False):
        params = []
        name = ""

        if toPointer:
            param = "&({0})".format(param)

        typeid = get_basic_type(typeid)
        if typeid in extended_types:
            name = "Ice_InputStream_read{0}".format(typeid.capitalize())

        if typeid in ["Ice_Int"]:
            name = "Ice_InputStream_read{0}".format(typeid[4:])

        if typeid == "string":
            name = "Ice_InputStream_readIceString"

        if typeid == "char*":
            name = "Ice_InputStream_readString"

        if kind in ["enum"]:
            name = "Ice_InputStream_readByte"
            param = "(Ice_Byte*){0}".format(param)

        if kind in ["proxy"]:
            name = "Ice_ObjectPrx_readFromInputStream"
            params = [param, self.ref()]

        if name and not params:
            params = [self.ref(), param]

        if not params:
            name = typeid + "_readFromInputStream"
            params = [param, self.ref()]

        self.holder += FunctionCall(name, *params)

    def ref(self):
        if self.is_pointer:
            return self.this
        return "&" + self.this

    def alloca(self, typeid, dest, size):
        sizeof = "sizeof({0}) * {1}".format(str(typeid), size)
        casting = Pointer(typeid)
        data_name = (
            dest.strip()
            .replace(".", "_")
            .replace("&", "")
            .replace("*", "")) + "_data"

        if args.use_alloca:
            alloca = FunctionCall("alloca", sizeof)
            alloca.result(dest, casting_to=casting)

            self.holder += alloca
        else:
            # NOTE: could not avoid declaring _data here, size is unknown until now.
            expr = "byte {}[{}];\n".format(data_name, sizeof)
            expr += "{} = ({}){};\n".format(dest, casting, data_name)
            self.holder += expr


class ForLoop(CCode):
    def __init__(self, varname, initial, condition, increment, type_="int"):
        self.varname = varname
        self.counter = Param(type_, varname)

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


class WhileLoop(CCode):
    def __init__(self, cond):
        self.cond = cond

    def __str__(self):
        retval = "while ({}) {{\n".format(self.cond)

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


class Switch(CCode):
    def __init__(self, arg):
        self.arg = arg

    def __str__(self):
        retval = "switch (" + self.arg + ") {\n"

        self.tab = 1
        for i in self:
            retval += str(i)

        retval = retval[:-1] + "}\n"
        return retval

    def end(self):
        self.append("break;\n")

    def case(self, *args):
        c = getattr(self, 'nested', None) or self

        class Context:
            def __enter__(self):
                c.nested = SwitchCase(args)

            def __exit__(self, type, value, traceback):
                nested = c.nested
                c.nested = None
                c.append(nested)

        return Context()


class SwitchCase(CCode):
    def __init__(self, args):
        self.args = args

    def __str__(self):
        retval = ""

        self.tab = 1
        for v in self.args:
            retval += "case " + str(v) + ":\n"
        retval = retval[:-1] + " {\n"

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
        includes.add_system("IceC/IceC.h")
        includes.add_system("IceC/IceUtil.h")

        if args.use_alloca:
            includes.add_system("alloca.h")
        c += includes
        c.nl()

    def get_output_filename(self):
        name = os.path.basename(self.args.input)
        name = os.path.splitext(name)[0]
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

    def cpp2c_name(self, name, kind, add_prx=True):
        if name in basic_types:
            return name

        if name.startswith("::"):
            name = name[2:].replace("::", "_")
            if kind == 'type':
                name = name.strip('*')
                if add_prx:
                    name += 'Prx'

        return name

    def render(self, c):
        c += "Not implemented"
        c.nl()

    def render_ptr(self, c, name=None, nameptr=None):
        if name is None:
            name = self.full_name

        if nameptr is None:
            nameptr = self.full_name + "Ptr"

        c += Typedef(nameptr, Pointer(name))
        c.nl()

    def render_marshaller(self, c):
        c += self.get_marshall_func()
        c.nl()

    def render_unmarshaller(self, c):
        c += self.get_unmarshall_func()
        c.nl()

    def render_children(self, c):
        for i in self:
            i.render(c)

    def get_marshall_func(self):
        name = self.full_name + "_writeToOutputStream"
        this = Param(Pointer(self.full_name, True), "self")
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
        this = Param(Pointer(self.full_name, True), "self")
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
        parents = ptr.scope()[2:].replace("::", "_")
        self.name = parents + ptr.name()


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
    def __init__(self, module, ptr, has_forward=False, bases=None):
        BaseDecl.__init__(self, module, ptr)
        self.has_forward = has_forward
        self.bases = bases

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
        st = Struct(self.full_name)

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
        this = Param(Pointer(self.full_name, True), "self")
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
        this = Param(Pointer(self.full_name, True), "self")
        func = (Function()
                .static().void()
                .name(name)
                .param(this))

        handler = self.full_name + "_methodHandler"
        func += FunctionCall("Ice_Object_init",
                             Casting("Ice_ObjectPtr", "self"),
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
        this = Param(Pointer(self.full_name, True), "self")
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

        operationSize = Param("Ice_Int", "operationSize", scope=func)
        func += operationSize.readFromStream("is", opargs=["false"], as_size=True)
        func += Assert("operationSize <= MAX_OPERATION_NAME_SIZE")
        func.nl()

        operation = ParamArray("char", "MAX_OPERATION_NAME_SIZE", "operation", scope=func)
        func += operation.readFromStream("is")
        func.nl()

        func += Comment(" FIXME: operationMode ignored ")
        mode = Param("byte", "mode", scope=func)
        func += mode.readFromStream("is")
        func.nl()

        func += Comment(" FIXME: Context not supported ")
        contextSize = Param("byte", "contextSize", scope=func)
        func += contextSize.readFromStream("is")
        func += Assert("contextSize == 0")
        func.nl()

        encapSize = Param("Ice_Int", "encapSize", scope=func)
        func += encapSize.readFromStream("is")

        major = Param("byte", "major", scope=func)
        minor = Param("byte", "minor", scope=func)
        func += major.readFromStream("is")
        func += minor.readFromStream("is")

        func += Assert("major == 1 && minor == 0")
        func += Assert("encapSize - 6 == (is->size - (is->next - is->data))")
        func.nl()

        # render own method calls
        for i in self:
            cond = IfCondition('strcmp(operation, "{0}") == 0'.format(i.name))
            cond += FunctionCall(i.full_name + "__dispatch", "self", "is", "os")
            cond.ret()
            func += cond
            func.nl()

        # render inherited method calls
        for typeid, i in self.inherited():
            cond = IfCondition('strcmp(operation, "{0}") == 0'.format(i.name))
            selfp = "({}Ptr)self".format(typeid)
            cond += FunctionCall(i.full_name + "__dispatch", selfp, "is", "os")
            cond.ret()
            func += cond
            func.nl()

        func += ThrowException("Ice_OperationNotExistException", "__FILE__", "__LINE__")
        c += func
        c.nl()

    def render_proxy_methods(self, c):
        for i in self:
            i.render_as_proxy_method(c)

    def inherited(self):
        operations = []
        for iface in self.bases:
            for op in iface:
                operations.append((iface.full_name, op))
        return operations


# FIXME: mix this with Param
class ParamDecl(BaseDecl):
    def __init__(self, st, ptr):
        self.name = ptr.name()
        self.type = TypeDecl(st, ptr.type())

        self.contained_type = None
        if self.type.kind == "sequence":
            self.contained_type = TypeDecl(st, ptr.type().type())

        self.is_proxy = self.get_is_proxy(st)

    def get_is_proxy(self, st):
        if self.type.typeid == "::Ice::Object*":
            return True

        if st.is_interface(self.cpp2c_name(self.type.typeid, self.type.kind)):
            return True

        return False

    def __str__(self):
        typeid = str(self.type)
        prx = ""
        if self.is_proxy:
            prx = "Ptr"

        return "{0}{1} {2}".format(typeid, prx, self.name)

    def declare(self):
        retval = CCode()
        typeid = str(self.type)
        param = Param(typeid, self.name, kind=self.type.kind)
        retval += param.declare()

        return retval


class OperationDecl(BaseDecl):
    def __init__(self, st, ptr):
        iface = st.top
        self.name = ptr.name()
        self.mode = ptr.mode()
        self.iface_name = iface.full_name
        self.full_name = self.iface_name + "_" + self.name
        self.retval = TypeDecl(st, ptr.returnType())

        self.params = []
        for p in ptr.parameters():
            self.params.append(ParamDecl(st, p))

    def render_as_servant_method(self, c):
        name = self.iface_name + "I_" + self.name
        this = Param(Pointer(self.iface_name, True), "self")

        func = (Function(True)
                .name(name)
                .param(this)
                .weak())

        params = self.params[:]

        if not self.retval.is_interface:
            func.retval(self.retval)
        else:
            retval = Param(Pointer(str(self.retval), True), 'retval')
            params.append(retval)

        for p in params:
            func.param(p)

        c += func
        c.nl()

    def render_as_dispatcher(self, c):
        name = self.iface_name + "_" + self.name + "__dispatch"
        this = Param(Pointer(self.iface_name, True), "self")
        ins = Param(Pointer("Ice_InputStream", True), "is")
        outs = Param(Pointer("Ice_OutputStream", True), "os")

        func = (Function()
                .static()
                .name(name)
                .param(this)
                .param(ins)
                .param(outs))

        # check if handler is defined (return otherwise)
        method_name = self.iface_name + "I_" + self.name
        with func.condition('&{} == 0'.format(method_name)):
            func.ret()

        # unmarshall params
        call_args = ["self"]
        for i in self.params:
            func.declare_var(i)

            ins = InputStream("is")
            kind = "proxy" if i.is_proxy else i.type.kind

            # if sequence, reserve space to hold it
            if kind in ["sequence", "dictionary"]:
                func.nl().comment(" allocate space for {} ".format(kind))
                size_var = i.name + ".size"
                items_var = i.name + ".items"

                size_var_ptr = "&" + size_var
                if kind == "dictionary":
                    size_var_ptr = "(Ice_Int*)" + size_var_ptr
                ins.readSize(size_var_ptr, "false")

                if kind == "sequence":
                    alloca_type = i.contained_type.typeid
                else:
                    alloca_type = i.type.typeid + "_Pair"

                ins.alloca(self.cpp2c_name(alloca_type, i.type.kind), items_var, size_var)

            ins.read(self.cpp2c_name(i.type.typeid, i.type.kind), "&" + i.name, kind)
            func += ins
            func.nl()

            arg = i.name
            if i.is_proxy:
                arg = "&" + arg
            call_args.append(arg)

        # if retval is Prx, add as output param
        retval = self.get_retval_param()
        if self.retval.is_interface:
            call_args.append('&' + retval.name)

        # call to servant method
        method = FunctionCall(method_name, *call_args)

        if self.retval.typeid != "void":
            func.declare_var(retval)
            if not self.retval.is_interface:
                method.result(retval.name)

        func += method

        # write result on output stream
        if self.retval.typeid != "void":
            kind = 'proxy' if self.retval.is_interface else self.retval.kind
            outs = OutputStream("os")
            outs.write(
                self.cpp2c_name(self.retval.typeid, self.retval.kind),
                retval.name,
                kind = kind,
                toPointer = True if kind == 'proxy' else False)
            func += outs

        c += func
        c.nl()

    def render_as_proxy_method(self, c):
        name = self.iface_name + "_" + self.name
        this = Param(Pointer(self.iface_name + "Prx", True), "self")
        retval = self.get_retval_param()

        func = (Function()
                .static()
                .name(name)
                .param(this))

        params = self.params[:]

        if not self.retval.is_interface:
            func.retval(self.retval)
        else:
            retval_ptr = Param(Pointer(str(self.retval), True), 'retval')
            params.append(retval_ptr)

        for p in params:
            func.param(p)

        func += Ptr_check(this)
        func.nl()

        mode = "Ice_{}".format(str(self.mode))
        func += FunctionCall("Ice_ObjectPrx_connect", "self")
        outs = OutputStream("os", "&(self->stream)", scope=func)
        outs.init()

        outs.nl().comment(" request header ")
        outs.writeBlob("Ice_header", "sizeof(Ice_header)")
        outs.nl().comment(" request body ")

        # FIXME: 0 must be for oneway/datagram invocations, >0 for twoway
        # FIXME: use 'else' branching
        with outs.condition('self->mode == Ice_ReferenceModeTwoway'):
            outs.write("int", 1, toPointer=False)          # request id
        with outs.condition('self->mode != Ice_ReferenceModeTwoway'):
            outs.write("int", 0, toPointer=False)          # request id

        outs.nl()
        outs.write("Ice_Identity", "self->identity")       # object identity
        outs.writeString("")                               # facet, only empty allowed
        outs.writeString(self.name)                        # operation
        outs.write("byte", mode, toPointer=False)          # operation mode
        outs.write("byte", 0, toPointer=False)             # context, not supported

        outs.nl().comment(" encapsulated params ")
        outs.startWriteEncaps()

        for p in self.params:
            kind = "proxy" if p.is_proxy else p.type.kind
            outs.write(self.cpp2c_name(p.type.typeid, p.type.kind),
                       p.name, kind=kind, toPointer=False)
        outs.endWriteEncaps()
        outs.setMessageSize()

        func += outs
        func.nl()

        func += FunctionCall("Ice_ObjectPrx_send", "self")
        func.nl()

        # parse reply message
        func.comment(" parse reply message (only on twoway invocations) ")
        with func.condition('self->mode == Ice_ReferenceModeTwoway'):
            ins = InputStream("is", scope=func, local=True, is_pointer=False)
            expected = ["'I'", "'c'", "'e'", "'P'", '1', '0', '1', '0', 'Ice_replyMsg', '0']
            expected = ParamArray("char", 10, "expected", expected, scope=func, local=True)
            size = Param("Ice_Int", "size", scope=func, local=True)
            requestId = Param("Ice_Int", "requestId", scope=func, local=True)
            replyStatus = Param("Ice_Byte", "replyStatus", scope=func, local=True)
            encapsSize = Param("Ice_Int", "encapsSize", scope=func, local=True)
            skipByte = Param("Ice_Byte", "skipByte", scope=func, local=True)
            func.nl()

            timeout = Param("uint16_t", "timeout", scope=func, local=True, value=5000)
            ready = Param("bool", "ready", scope=func, local=True, value="false")

            loop = WhileLoop("!ready")
            loop += FunctionCall(
                "IcePlugin_Connection_dataReady", "&(self->connection)", ready.ref())
            loop += FunctionCall("IceUtil_delay", 1)
            with loop.condition("! --timeout"):
                loop += ThrowException("Ice_TimeoutException", "__FILE__", "__LINE__")
                self.render_return(loop)
            func += loop

            func.nl().comment(" reply header ")
            ins.init("self->connection.fd", "self->connection.epinfo->type")
            with func.condition("memcmp(expected, is.next, 10) != 0"):
                func += ThrowException("Ice_ProtocolException", "__FILE__", "__LINE__")
                self.render_return(func)

            func.nl().comment(" skip header from input stream ")
            func += "is.next += 10;\n"
            func += size.readFromStream(ins)

            with func.condition("size > MAX_MESSAGE_SIZE"):
                func += ThrowException("Ice_MemoryLimitException", "__FILE__", "__LINE__")
                self.render_return(func)

            func.nl().comment(" reply body ")
            func += requestId.readFromStream(ins)
            func += replyStatus.readFromStream(ins)

            func.nl().comment(" check replyStatus ")
            with func.switch("replyStatus") as sw:
                with sw.case(0):
                    sw.comment(" success ")
                    sw.end()

                with sw.case(1):
                    sw.comment(" user exception ")
                    sw += FunctionCall("Ice_ObjectPrx_parseUserException", "self", "&is")
                    self.render_return(sw)

                for i, v in enumerate(["Object", "Facet", "Operation"], 2):
                    with sw.case(i):
                        sw += ThrowException("Ice_" + v + "DoesNotExist",
                                             "__FILE__", "__LINE__")
                        self.render_return(sw)

                with sw.case(5, 6, 7):
                    sw.comment(" unkown exception (reason as string) ")
                    sw.comment(" FIXME: unmarshall and print reason ")
                    sw += ThrowException("Ice_UnknownException", "__FILE__", "__LINE__")
                    self.render_return(sw)

            func.nl()
            func += encapsSize.readFromStream(ins)

            # FIXME: what are these??
            func += skipByte.readFromStream(ins)
            func += skipByte.readFromStream(ins)

            # FIXME: convert type properly
            # FIXME: initialize it to a default value
            if self.retval.typeid != "void":
                if self.retval.is_interface:
                    func += retval.readFromStream(ins, is_pointer=True)
                else:
                    func.declare_var(retval)
                    func += retval.readFromStream(ins)

        func.nl()
        func += FunctionCall("Ice_ObjectPrx_closeConnection", "self")
        self.render_return(func, False)

        c += func
        c.nl()

    def get_retval_param(self):
        if self.retval.typeid in extended_types:
            retval_type = "Ice_" + str(self.retval.typeid).capitalize()
        else:
            retval_type = self.cpp2c_name(self.retval.typeid, self.retval.kind)
        kind = 'proxy' if self.retval.is_interface else self.retval.kind
        return Param(retval_type, "retval", kind=kind)


    def render_return(self, func, force_return=True):
        if self.retval.typeid != "void" and not self.retval.is_interface:
            retval = self.get_retval_param()
            func.ret(retval)
        elif force_return:
            func.ret()


class TypeDecl(BaseDecl):
    def __init__(self, st, ptr):
        self.typeid = "void"
        self.kind = None

        if ptr is not None:
            self.typeid = ptr.typeId()
            self.kind = str(ptr.__class__.__name__).lower()

        name = self.cpp2c_name(self.typeid, self.kind, False)
        self.is_interface = st.is_interface(name)

    def __repr__(self):
        if self.typeid in extended_types:
            return "Ice_" + self.typeid.capitalize()

        return self.cpp2c_name(self.typeid, self.kind)


class SequenceDecl(BaseDecl):
    def __init__(self, st, ptr):
        BaseDecl.__init__(self, st.module, ptr)
        self.type = TypeDecl(st, ptr.type())

    def render(self, c):
        self.render_sequence(c)
        self.render_ptr(c)
        self.render_marshaller(c)
        self.render_unmarshaller(c)

    def render_sequence(self, c):
        seq = Struct(self.full_name)

        size = Param("Ice_Int", "size")
        seq.append(size)
        items = Param(Pointer(self.type), "items")
        seq.append(items)

        c += Typedef(self.full_name, seq)
        c.nl()

    def render_marshaller(self, c):
        fn = self.get_marshall_func()

        # write size of sequence
        outs1 = OutputStream("os")
        outs1.writeSize("self->size")
        fn += outs1
        fn.nl()

        # write items of sequence
        outs2 = OutputStream("os")
        outs2.write(
            self.cpp2c_name(self.type.typeid, self.type.kind),
            "*(self->items + i)",
            self.type.kind,
            toPointer = False)

        loop = ForLoop("i", "0", "i<self->size", "i++")
        loop += outs2
        fn += loop
        fn.declare_var(loop.counter)

        c += fn
        c.nl()

    def render_unmarshaller(self, c):
        fn = self.get_unmarshall_func()

        # read size of sequence
        ins1 = InputStream("is")
        ins1.readSize("&(self->size)", "true")
        fn += ins1
        fn.nl()

        # zero-copy optimization, for byte only
        if self.type.typeid == "byte":
            fn.comment(" zero-copy optimization ")
            fn += "self->items = is->next;\n"
            fn += "is->next += self->size;\n"

        # read items of sequence
        else:
            ins2 = InputStream("is")
            ins2.read(
                self.cpp2c_name(self.type.typeid, self.type.kind),
                "self->items + i",
                self.type.kind)

            loop = ForLoop("i", "0", "i<self->size", "i++")
            loop += ins2
            fn += loop
            fn.declare_var(loop.counter)

        c += fn
        c.nl()


class DictionaryDecl(BaseDecl):
    def __init__(self, st, ptr):
        BaseDecl.__init__(self, st.module, ptr)
        self.key_type = TypeDecl(st, ptr.keyType())
        self.value_type = TypeDecl(st, ptr.valueType())

    def render(self, c):
        self.render_dictionary_pair(c)
        self.render_dictionary(c)
        self.render_init_method(c)
        self.render_set_method(c)
        self.render_marshaller(c)
        self.render_unmarshaller(c)

    def render_dictionary_pair(self, c):
        name = self.full_name + "_Pair"
        pair = Struct(name)

        key = Param(self.key_type, "key")
        pair.append(key)
        value = Param(self.value_type, "value")
        pair.append(value)

        c += Typedef(name, pair)
        c.nl()

        self.render_ptr(c, name, name + "Ptr")

    def render_dictionary(self, c):
        dic = Struct(self.full_name)

        size = Param("byte", "size")
        dic.append(size)
        items = Param(self.full_name + "_PairPtr", "items")
        dic.append(items)

        c += Typedef(self.full_name, dic)
        c.nl()

        self.render_ptr(c)

    def render_init_method(self, c):
        init = Alias(self.full_name + "_init", "Ice_Dict_init")
        c += init
        c.nl()

    def render_set_method(self, c):
        setm = Macro(self.full_name + "_set", "DICT", "INDEX", "KEY", "VALUE")
        tab = " " * 4

        setm += tab
        key_type = self.key_type.typeid
        if key_type == "string":
            setm += FunctionCall("Ice_String_init", "DICT.items[INDEX].key", "KEY")
        else:
            setm += FunctionCall("Ice_Dict_set", "DICT.items[INDEX].key", "KEY")

        value_type = self.value_type.typeid
        setm += tab
        if value_type == "string":
            setm += FunctionCall("Ice_String_init", "DICT.items[INDEX].value", "VALUE")
        else:
            setm += FunctionCall("Ice_Dict_set", "DICT.items[INDEX].value", "VALUE")

        setm = str(setm)
        longest = 0
        for l in setm.splitlines():
            longest = max(longest, len(l))

        result = ""
        for l in setm.splitlines():
            result += l + " " * (longest - len(l)) + "    \\\n"

        c += result[:-2] + "\n"
        c.nl()

    def render_marshaller(self, c):
        fn = self.get_marshall_func()

        # write size of dictionary
        outs1 = OutputStream("os")
        outs1.writeSize("self->size")
        fn += outs1
        fn.nl()

        # write items of dictionary
        outs2 = OutputStream("os")
        outs2.write(
            self.cpp2c_name(self.key_type.typeid, self.key_type.kind),
            "self->items[i].key",
            self.key_type.kind,
            toPointer=False)

        outs2.write(
            self.cpp2c_name(self.value_type.typeid, self.value_type.kind),
            "self->items[i].value",
            self.value_type.kind,
            toPointer=False)

        loop = ForLoop("i", "0", "i<self->size", "i++", type_="byte")
        loop += outs2
        fn += loop
        fn.declare_var(loop.counter)

        c += fn
        c.nl()

    def render_unmarshaller(self, c):
        fn = self.get_unmarshall_func()

        size = Param("Ice_Int", "size")
        fn.declare_var(size)

        # read size of dictionary
        ins1 = InputStream("is")
        ins1.readSize("&size", "true")
        fn += ins1
        fn += Assert("size < 256")
        fn += "self->size = size;\n"
        fn.nl()

        # read items of dictionary
        ins2 = InputStream("is")
        ins2.read(
            self.cpp2c_name(self.key_type.typeid, self.key_type.kind),
            "&(self->items[i].key)",
            self.key_type.kind)

        ins2.read(
            self.cpp2c_name(self.value_type.typeid, self.value_type.kind),
            "&(self->items[i].value)",
            self.value_type.kind)

        loop = ForLoop("i", "0", "i<self->size", "i++", type_="byte")
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
            member = Bunch(typeid = get_ice_type(type.typeId()),
                           name = i.name(),
                           kind = str(type.__class__.__name__).lower())
            self.members.append(member)

    def render(self, c):
        self.render_struct(c)
        self.render_ptr(c)
        self.render_marshaller(c)
        self.render_unmarshaller(c)

    def render_struct(self, c):
        struct = Struct(self.full_name)
        for m in self.members:

            typeid = m.typeid
            if m.typeid == "string":
                typeid = "Ice_String"

            p = Param(self.cpp2c_name(typeid, m.kind), m.name)
            struct.append(p)

        c += Typedef(self.full_name, struct)
        c.nl()

    def render_marshaller(self, c):
        fn = self.get_marshall_func()

        outs = OutputStream("os")
        for m in self.members:
            param = "self->{0}".format(m.name)
            outs.write(self.cpp2c_name(m.typeid, m.kind), param, m.kind, toPointer=False)
        fn += outs

        c += fn
        c.nl()

    def render_unmarshaller(self, c):
        fn = self.get_unmarshall_func()

        ins = InputStream("is")
        initialized = False
        for m in self.members:
            param = "self->{0}".format(m.name)

            if m.typeid in basic_types + ['char']:
                fn += "{0} = 0;\n".format(param)
                initialized = True

            ins.read(self.cpp2c_name(m.typeid, m.kind), param, m.kind, toPointer=True)

        if initialized:
            fn.nl()

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
    def __init__(self, args):
        self._args = args

        self.items = []
        self.ifaces = []
        self.declared = []
        self.module_stack = []

    def __iter__(self):
        return iter(self.items)

    @property
    def top(self):
        return self.items[-1]

    def is_interface(self, typeid):
        for i in self.ifaces:
            if i.full_name == typeid:
                return True
        return typeid == 'Ice_Object'

    def visitModuleStart(self, moduleptr):
        self.module = ModuleDecl(moduleptr)
        self.module_stack.append(moduleptr)
        return True

    def visitModuleEnd(self, moduleptr):
        self.module_stack.pop()
        if self.module_stack:
            self.module = ModuleDecl(self.module_stack[-1])
        return True

    def visitClassDecl(self, classdeclptr):
        self.ifaces.append(ClassDefDecl(self.module, classdeclptr))

        full_name = "{}_{}".format(self.module.name, classdeclptr.name())
        if full_name not in self.declared:
            self.declared.append(full_name)
            self.items.append(ClassDefForward(self.module, classdeclptr))

    def visitClassDefStart(self, classdefptr):
        name = classdefptr.name()
        full_name = "{}_{}".format(self.module.name, name)
        already_declared = full_name in self.declared

        bases = []

        for b in classdefptr.allBases():
            base_full_name = b.scope().strip("::").replace("::", "_")
            base_full_name += "_" + b.name()
            for i in self.ifaces:
                if i.full_name == base_full_name:
                    bases.append(i)
                    break

        cls = ClassDefDecl(self.module, classdefptr, already_declared, bases)

        if args.ifaces is not None:
            if cls.full_name not in args.ifaces:
                return False

        self.items.append(cls)
        self.ifaces.append(cls)

        if not already_declared:
            self.declared.append(full_name)

        return True

    def visitStructStart(self, structptr):
        self.items.append(StructDecl(self.module, structptr))
        return True

    def visitOperation(self, operationptr):
        self.top.append(OperationDecl(self, operationptr))

    def visitSequence(self, sequenceptr):
        self.items.append(SequenceDecl(self, sequenceptr))

    def visitDictionary(self, dictionaryptr):
        self.items.append(DictionaryDecl(self, dictionaryptr))

    def visitEnum(self, enumptr):
        self.items.append(EnumDecl(self.module, enumptr))


if __name__ == "__main__":
    import argparse

    args = argparse.ArgumentParser()
    args.add_argument('input', help='Slice file')
    args.add_argument('-I', dest='includes', action='append',
                      help="Add include search path")
    args.add_argument('--iface', dest='ifaces', action='append',
                      help="Specify the interface to generate")
    args.add_argument('--use-alloca', action="store_true",
                      help="Use alloca() instead of dynamic memory")
    args.add_argument('--ice', action="store_true",
                      help="Allow reserved Ice prefix in Slice identifiers")
    args = args.parse_args()

    parser = sliceparser.Parser(args.input)
    if args.ice:
        parser.addArgument("--ice")

    if args.includes is not None:
        for p in args.includes:
            parser.addArgument("-I" + p)

    visitor = IceVisitor(args)
    builder = IceCBuilder(args)

    parser.accept(visitor)
    builder.build(visitor)

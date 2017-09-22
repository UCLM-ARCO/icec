/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/IceUtil.h>

void ICEC_FUNC_ATTR
Object_init(ObjectPtr self) {
    self->tag = TAG_OK;
}

void ICEC_FUNC_ATTR
Ice_Object_init(Ice_ObjectPtr self, Ice_MethodHandlerPtr handler) {
    trace();
    self->handler = handler;
    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_Blobject_dispatch(Ice_ObjectPtr self, Ice_InputStreamPtr is, Ice_OutputStreamPtr os) {
    Ice_Int operationSize = 0;
    byte identity_size = 0;
    char identity[MAX_IDENTITY_SIZE];
    char operation[MAX_OPERATION_NAME_SIZE];
    byte mode = 0;
    byte contextSize = 0;
    Ice_Int encapSize = 0;
    byte major = 0;
    byte minor = 0;
    Ice_Current current;

    trace();

    Ptr_check(self);
    Ptr_check(is);
    Ptr_check(os);

    Ice_InputStream_readSize(is, &operationSize, false);
    assert(operationSize <= MAX_OPERATION_NAME_SIZE);

    Ice_InputStream_readString(is, operation);

    /* FIXME: operationMode ignored */
    Ice_InputStream_readByte(is, &mode);

    /* FIXME: Context not supported */
    Ice_InputStream_readByte(is, &contextSize);
    assert(contextSize == 0);

    Ice_InputStream_readInt(is, &encapSize);
    Ice_InputStream_readByte(is, &major);
    Ice_InputStream_readByte(is, &minor);
    assert(major == 1 && minor == 0);
    assert(encapSize - 6 == (is->size - (is->next - is->data)));

    /* call user 'ice_invoke' handler */
    identity_size = *(is->data + 18);
    strncpy(identity, (const char*)(is->data + 19), identity_size);
    identity[identity_size] = 0;
    current.id = identity;
    current.operation = operation;

    ((Ice_BlobjectPtr)self)->user_handler((Ice_BlobjectPtr)self, is, current, os);
}

void ICEC_FUNC_ATTR
Ice_Blobject_init(Ice_BlobjectPtr self, Ice_Blobject_HandlerPtr handler) {
    trace();
    self->user_handler = handler;
    Ice_Object_init((Ice_ObjectPtr)self, &Ice_Blobject_dispatch);
}

void ICEC_FUNC_ATTR
Ice_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) {
    trace();

    /* print_dbg("is init\n"); */

    IcePlugin_InputStream_init(self, fd, type);

    self->next = self->data;
    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_InputStream_readBool(Ice_InputStreamPtr self, Ice_Bool* v) {
    trace();
    Ptr_check(self);

    *v = *(self->next++) != 0;
}

void ICEC_FUNC_ATTR
Ice_InputStream_readByte(Ice_InputStreamPtr self, Ice_Byte* v) {
    trace();
    Ptr_check(self);

    *v = *(self->next++);
}

void ICEC_FUNC_ATTR
Ice_InputStream_readShort(Ice_InputStreamPtr self, Ice_Short* v) {
    Ice_Byte* src;
    Ice_Byte* dest;

    trace();
    Ptr_check(self);

    if (self->next - self->data + sizeof(Ice_Short) > (unsigned int)self->size) {
        c_throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
        return;
    }

    src = self->next;
    self->next += sizeof(Ice_Short);

#ifdef ICE_BIG_ENDIAN
    dest = (Ice_Byte*)(v + sizeof(Ice_Short) - 1);
    *dest-- = *src++;
    *dest = *src;
#else
    dest = (Ice_Byte*)v;
    *dest++ = *src++;
    *dest = *src;
#endif
}

void ICEC_FUNC_ATTR
Ice_InputStream_readInt(Ice_InputStreamPtr self, Ice_Int* v) {
    Ice_Byte* src;
    Ice_Byte* dest;

    trace();
    Ptr_check(self);

    if (self->next - self->data + sizeof(Ice_Int) > (unsigned int)self->size) {
        c_throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
        return;
    }

    src = self->next;
    self->next += sizeof(Ice_Int);

#ifdef ICE_BIG_ENDIAN
    dest = (Ice_Byte*)(v + sizeof(Ice_Int) - 1);
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest = *src;
#else
    dest = (Ice_Byte*)v;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest = *src;
#endif
}

void ICEC_FUNC_ATTR
Ice_InputStream_readFloat(Ice_InputStreamPtr self, Ice_Float* v) {
    Ice_Byte* src;
    Ice_Byte* dest;

    trace();
    Ptr_check(self);

    /* hexdump(NULL, self->next, 4); */

    if (self->next - self->data + sizeof(Ice_Float) > (unsigned int)self->size) {
        c_throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
        return;
    }

    src = self->next;
    self->next += sizeof(Ice_Float);

#ifdef ICE_BIG_ENDIAN
    dest = (Ice_Byte*)(v + sizeof(Ice_Float) - 1);
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest = *src;
#else
    dest = (Ice_Byte*)v;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest = *src;
#endif
}

void ICEC_FUNC_ATTR
Ice_InputStream_readSize(Ice_InputStreamPtr self, Ice_Int* size, bool increment) {
    Ice_Byte b = 0;

    trace();
    Ptr_check(self);

    Ice_InputStream_readByte(self, &b);

    if (b < 255) {
        *size = b;
        if (increment == false) {
            self->next--;
        }
        return;
    }

    Ice_InputStream_readInt(self, size);
    if (increment == false)
        self->next -= (sizeof(Ice_Int) + 1);
    assert(*size >= 0);
}

void ICEC_FUNC_ATTR
Ice_InputStream_readString(Ice_InputStreamPtr self, char* v) {
    Ice_Int size = 0;

    trace();
    Ptr_check(self);

    Ice_InputStream_readSize(self, &size, true);
    if (v != NULL)  {
        strncpy(v, (char*)self->next, size);
        v[size] = 0;
    }
    self->next += size;
}

void ICEC_FUNC_ATTR
Ice_InputStream_readIceString(Ice_InputStreamPtr self, Ice_StringPtr s) {
    Ice_Int size = 0;

    trace();
    Ptr_check(self);

    Ice_InputStream_readSize(self, &size, true);
    if (s != NULL)  {
        s->value = (char*)self->next;
        s->size = size;
    }
    self->next += size;
}

void ICEC_FUNC_ATTR
Ice_InputStream_readBlob(Ice_InputStreamPtr self, byte* v, int size) {
    trace();
    Ptr_check(self);

    assert(self->size - (self->next - self->data) >= size);

    memcpy(v, self->next, size);
    self->next += size;
}

void ICEC_FUNC_ATTR
Ice_InputStream_readIdentity(Ice_InputStreamPtr self, char* name) {
    trace();
    Ptr_check(self);

    Ice_InputStream_readString(self, name); /* name */
    Ice_InputStream_readString(self, NULL); /* category (not supported) */
}

void ICEC_FUNC_ATTR
Ice_OutputStream_init(Ice_OutputStreamPtr self) {
    trace();
    self->size = 0;
    self->next = self->data;
    self->encaps[0] = NULL;
    self->current_encaps = 0;

    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeBool(Ice_OutputStreamPtr self, Ice_Bool v) {
    trace();
    Ptr_check(self);

    assert((self->size + 1 <= MAX_MESSAGE_SIZE));

    *(self->next++) = v ? 1 : 0;
    self->size++;
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeByte(Ice_OutputStreamPtr self, Ice_Byte v) {
    trace();
    Ptr_check(self);

    assert((self->size + 1 <= MAX_MESSAGE_SIZE));

    *(self->next++) = v;
    self->size++;
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeShort(Ice_OutputStreamPtr self, Ice_Short v) {
    byte size;
    byte* dest;
    byte* src;

    trace();
    Ptr_check(self);

    size = sizeof(Ice_Short);
    assert((self->size + size <= MAX_MESSAGE_SIZE));

    dest = self->next;

#ifdef ICE_BIG_ENDIAN
    src = (byte*)&v + sizeof(Ice_Short) - 1;
    *dest++ = *src--;
    *dest = *src;
#else
    src = (byte*)&v;
    *dest++ = *src++;
    *dest = *src;
#endif

    self->next += size;
    self->size += size;
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeInt(Ice_OutputStreamPtr self, Ice_Int v) {
    byte size;
    byte* dest;
    byte* src;

    trace();
    Ptr_check(self);

    size = sizeof(Ice_Int);
    assert((self->size + size <= MAX_MESSAGE_SIZE) && "Too long message!");

    dest = self->next;

#ifdef ICE_BIG_ENDIAN
    src = (byte*)&v + sizeof(Ice_Int) - 1;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest = *src;
#else
    src = (byte*)&v;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest = *src;
#endif

    self->next += size;
    self->size += size;
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeFloat(Ice_OutputStreamPtr self, Ice_Float v) {

    byte size;
    byte* dest;
    byte* src;

    trace();
    Ptr_check(self);

    size = sizeof(Ice_Int);
    assert((self->size + size <= MAX_MESSAGE_SIZE) && "Too long message!");

    dest = self->next;

#ifdef ICE_BIG_ENDIAN
    src = (byte*)&v + sizeof(Ice_Float) - 1;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest = *src;
#else
    src = (byte*)&v;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest = *src;
#endif

    self->next += size;
    self->size += size;
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeBlob(Ice_OutputStreamPtr self, const byte* data, int size) {
    trace();
    Ptr_check(self);

    assert((self->size + size <= MAX_MESSAGE_SIZE) && "Too long message!");

    memcpy(self->next, data, size);
    self->next += size;
    self->size += size;
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeSize(Ice_OutputStreamPtr self, int size) {
    trace();
    Ptr_check(self);

    assert (size >= 0);
    if (size > 254) {
        Ice_OutputStream_writeByte(self, 255);
        Ice_OutputStream_writeInt(self, size);
    }
    else {
        Ice_OutputStream_writeByte(self, (byte)size);
    }
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeString(Ice_OutputStreamPtr self, const char* str) {
    int size;

    trace();
    Ptr_check(self);

    size = strlen(str);
    Ice_OutputStream_writeSize(self, size);
    Ice_OutputStream_writeBlob(self, (byte*)str, size);
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeIceString(Ice_OutputStreamPtr self, Ice_String str) {
    trace();
    Ptr_check(self);

    Ice_OutputStream_writeSize(self, str.size);
    Ice_OutputStream_writeBlob(self, (byte*)str.value, str.size);
}

void ICEC_FUNC_ATTR
Ice_OutputStream_writeIdentity(Ice_OutputStreamPtr self, const char* id) {
    trace();
    Ptr_check(self);

    if (strcspn(id, "/") != strlen(id)) {
        fixme("category not supported");
    }

    Ice_OutputStream_writeString(self, id); /* name */
    Ice_OutputStream_writeByte(self, 0);    /* category */
}

void ICEC_FUNC_ATTR
Ice_OutputStream_startWriteEncaps(Ice_OutputStreamPtr self) {
    trace();
    Ptr_check(self);

    assert(self->current_encaps + 1 < MAX_NESTED_ENCAPS);
    self->encaps[++(self->current_encaps)] = self->next;

    Ice_OutputStream_writeInt(self, 0); /* placeholder for encapsulation length */
    Ice_OutputStream_writeByte(self, encodingMajor);
    Ice_OutputStream_writeByte(self, encodingMinor);
}

void ICEC_FUNC_ATTR
Ice_OutputStream_endWriteEncaps(Ice_OutputStreamPtr self) {
    Ice_Int* size;

    trace();
    Ptr_check(self);

#ifdef ESP8266
    /* Writting a whole int (4 bytes) here could cause an Aligment exception (9).
       Reduce it to 4 writes of one byte each */
    Ice_Int value;
    byte* dst;
    byte* src;

    value = (Ice_Int)(self->next - self->encaps[self->current_encaps]);
    dst = (byte*)self->encaps[self->current_encaps];
    src = (byte*)&value;

    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst = *src;

#else

    size = (Ice_Int*)self->encaps[self->current_encaps];
    *size = (Ice_Int)(self->next - self->encaps[self->current_encaps]);
#endif

    self->encaps[(self->current_encaps)--] = NULL;
}

void ICEC_FUNC_ATTR
Ice_OutputStream_setMessageSize(Ice_OutputStreamPtr self) {
    byte* next;
    int size;

    trace();
    Ptr_check(self);

    next = self->next;
    size = self->size;

    /* Ice header is fixed length, size starts at position 11 */
    self->next = self->data + 10;
    Ice_OutputStream_writeInt(self, size);

    self->next = next;
    self->size = size;
}

void ICEC_FUNC_ATTR
Ice_Connection_init(Ice_ConnectionPtr self, Ice_EndpointInfoPtr epinfo) {
    trace();

    self->fd = -1;
    self->current_client = -1;
    self->epinfo = epinfo;
    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_Connection_send(Ice_ConnectionPtr self, byte* data, int size) {
    int fd, res;

    trace();
    Ptr_check(self);

    fd = self->current_client;
    if (fd == -1) fd = self->fd;

    assert(self->fd > -1);

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(data, size);
#endif

    res = IcePlugin_Connection_send(self, fd, data, size);
    if (res == size) {
        return;
    }

    /* FIXME: reconnect! */
    if (res == 0) {
        error_at_line(0, 0, __FILE__, __LINE__, "Connection lost!\n");
        return;
    }

    /* FIXME: reset device! */
    if (res == -1) {
        error_at_line(0, errno, __FILE__, __LINE__, "Error on socket");
        return;
    }

    /* FIXME: reset device! */
    error_at_line(0, 0, __FILE__, __LINE__, "Could not write all bytes on socket!\n");
}

void ICEC_FUNC_ATTR
Ice_Connection_sendValidate(Ice_ConnectionPtr self) {
    Ice_OutputStream os;

    trace();
    Ptr_check(self);

    Ice_OutputStream_init(&os);
    Ice_OutputStream_writeBlob(&os, Ice_header, 8);
    Ice_OutputStream_writeByte(&os, Ice_validateConnectionMsg);
    Ice_OutputStream_writeByte(&os, 0);  /* compression */
    Ice_OutputStream_writeInt(&os, 14);

    Ice_Connection_send(self, os.data, os.size);
}

void ICEC_FUNC_ATTR
Ice_Connection_receiveValidate(Ice_ConnectionPtr self) {
    Ice_InputStream is;

    char expected[] = {
        'I', 'c', 'e', 'P',        /* int  magic */
        1,                         /* byte protocolMajor */
        0,                         /* byte protocolMinor */
        1,                         /* byte encodingMajor */
        0,                         /* byte encodingMinor */
        Ice_validateConnectionMsg, /* byte messageType (0 -> request) */
        0,                         /* byte compressionStatus (0 -> no compress) */
        14, 0, 0, 0};              /* int size */

    trace();
    Ptr_check(self);

    Ice_InputStream_init(&is, self->fd, self->epinfo->type);

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(is.data, is.size);
#endif

    if (memcmp(expected, is.next, 14) != 0) {
        c_throw("Ice_ProtocolException", __FILE__, __LINE__);
        return;
    }
}

void ICEC_FUNC_ATTR
Ice_Connection_sendClose(Ice_ConnectionPtr self) {
    Ice_OutputStream os;

    trace();
    Ptr_check(self);

    Ice_OutputStream_init(&os);
    Ice_OutputStream_writeBlob(&os, Ice_header, 8);
    Ice_OutputStream_writeByte(&os, Ice_closeConnectionMsg);
    Ice_OutputStream_writeByte(&os, 0);  /* compression */
    Ice_OutputStream_writeInt(&os, 14);

    Ice_Connection_send(self, os.data, os.size);
}

void ICEC_FUNC_ATTR
Ice_ServantMap_init(Ice_ServantMapPtr self) {
    trace();
    self->count = 0;
    self->default_servant = NULL;
    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_ObjectAdapter_init(Ice_ObjectAdapterPtr self,
                       Ice_CommunicatorPtr communicator) {

    trace();
    Ptr_check(communicator);

    self->communicator = communicator;
    self->activated = false;

    Ice_Connection_init(&(self->connection), &(self->epinfo));
    Ice_ServantMap_init(&(self->servantMap));
    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    bool ok;

    trace();
    Ptr_check(self);

    ok = IcePlugin_ObjectAdapter_activate(self);
    if (! ok) {
        c_throw("Ice_NoEndpointException", __FILE__, __LINE__);
        return;
    }

    Ice_Communicator_registerAdapter(self->communicator, self);
    self->activated = true;
}

void ICEC_FUNC_ATTR
Ice_ObjectAdapter_add(Ice_ObjectAdapterPtr self,
                      Ice_ObjectPtr servant,
                      const char* identity) {
    Ice_ServantMapPtr sm;

    /* print_dbg("IceC: object added: "); */
    /* print_dbg(identity); */
    /* print_dbg("\n"); */

    trace();
    Ptr_check(self);
    Ptr_check(servant);

    assert(strlen(identity) < MAX_IDENTITY_SIZE);

    sm = &(self->servantMap);
    assert(sm->count < MAX_NUM_OF_SERVANTS);

    strcpy(sm->ids[sm->count], identity);
    sm->objects[sm->count] = servant;
    sm->count++;
}

void ICEC_FUNC_ATTR
Ice_ObjectAdapter_addDefaultServant(Ice_ObjectAdapterPtr self,
                                    Ice_ObjectPtr servant) {
    trace();
    Ptr_check(self);
    Ptr_check(servant);

    self->servantMap.default_servant = servant;

}

int ICEC_FUNC_ATTR
Ice_ObjectAdapter_getClient(Ice_ObjectAdapterPtr self) {
    trace();
    Ptr_check(self);

    /* print_dbg("getClient\n"); */

    if (! self->epinfo.datagram) {
        IcePlugin_Connection_accept(&(self->connection));
        Ice_Connection_sendValidate(&(self->connection));
        assert(self->connection.current_client != -1 && "Invalid client");
        return self->connection.current_client;
    }

    /* print_dbg(" - try\n"); */
    /* if (self->connection.fd == -1) */
    /*  print_dbg(" - bad client\n"); */

    assert(self->connection.fd != -1 && "Invalid client");
    /* print_dbg(" - ok\n"); */
    return self->connection.fd;
}

int ICEC_FUNC_ATTR
Ice_ObjectAdapter_closeClient(Ice_ObjectAdapterPtr self) {
    trace();
    Ptr_check(self);

    if (! self->epinfo.datagram) {
        Ice_Connection_sendClose(&(self->connection));
        IcePlugin_Connection_close(&(self->connection));
        self->connection.current_client = -1;
    }

    return 0;
}

void ICEC_FUNC_ATTR
Ice_ObjectAdapter_incomingMessage(Ice_ObjectAdapterPtr self) {
    int fd, i;
    Ice_Int size, requestId;
    Ice_InputStream is;
    char identity[MAX_IDENTITY_SIZE];
    byte facetSeqSize;
    Ice_ObjectPtr servant;

    char header[] = {
        'I', 'c', 'e', 'P',    /* int  magic */
        1,                     /* byte protocolMajor */
        0,                     /* byte protocolMinor */
        1,                     /* byte encodingMajor */
        0,                     /* byte encodingMinor */
        0,                     /* byte messageType (0 -> request) */
        0};                    /* byte compressionStatus (0 -> no compress) */

    trace();
    Ptr_check(self);

    /* { */
    /*     static int counter = 1; */
    /*     print_dbg("in message #"); */
    /*     printi_dbg(counter++); */
    /*     print_dbg("\n"); */
    /* } */

    fd = Ice_ObjectAdapter_getClient(self);
    Ice_InputStream_init(&is, fd, self->connection.epinfo->type);

    /* if (is.size) {
        print_dbg(" - contents: \n");
        hexdump(is.data, is.size);
    } */

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(is.data, is.size);
#endif

    if (memcmp(header, is.next, 10) != 0) {
        c_throw("Ice_ProtocolException", __FILE__, __LINE__);
        Ice_ObjectAdapter_closeClient(self);
        return;
    }
    is.next += 10;

    /* print_dbg("header ok\n"); */

    Ice_InputStream_readInt(&is, &size);

    if (size > MAX_MESSAGE_SIZE) {
        c_throw("Ice_MemoryLimitException", __FILE__, __LINE__);
        Ice_ObjectAdapter_closeClient(self);
        return;
    }

    /* print_dbg("size ok\n"); */

    Ice_InputStream_readInt(&is, &requestId);
    Ice_InputStream_readIdentity(&is, identity);

    /* print_dbg("oid: '"); */
    /* print_dbg(identity); */
    /* print_dbg("'\n"); */

    /* facet not supported */
    Ice_InputStream_readByte(&is, &facetSeqSize);

    assert(facetSeqSize == 0);

    /* use default servant when defined */
    servant = self->servantMap.default_servant;

    /* and search for an identified servant if available */
    for (i=0; i<self->servantMap.count; i++) {
        if (streq(self->servantMap.ids[i], identity)) {
            servant = self->servantMap.objects[i];
            break;
        }
    }

    if (servant == NULL) {
        c_throw("Ice_ObjectNotExistException", __FILE__, __LINE__);
        Ice_ObjectAdapter_closeClient(self);
        return;
    }

    /* servant found (named or default) */
    /* print_dbg("servant available\n"); */
    assert(servant->handler != NULL);

    /* prepare reply message */
    Ice_OutputStream os;
    Ice_OutputStream_init(&os);

    /* reply header (if twoway) */
    if (requestId > 0) {
        Ice_OutputStream_writeBlob(&os, Ice_header, 8);
        Ice_OutputStream_writeByte(&os, Ice_replyMsg);
        Ice_OutputStream_writeByte(&os, 0);  /* compression */
        Ice_OutputStream_writeInt(&os, 0);   /* size (placeholder) */

        /* reply data */
        Ice_OutputStream_writeInt(&os, requestId);
        Ice_OutputStream_writeByte(&os, 0);  /* reply status */

        /* user data encapsulation */
        Ice_OutputStream_startWriteEncaps(&os);
    }

    servant->handler(servant, &is, &os);
    /* print_dbg("servant called\n"); */

    if (requestId > 0) {
        Ice_OutputStream_endWriteEncaps(&os);
        Ice_OutputStream_setMessageSize(&os);
        Ice_Connection_send(&(self->connection), os.data, os.size);
    }

    /* FIXME: check if client has sent more messages. If so, */
    /* process them; otherwise, close connection */

    Ice_ObjectAdapter_closeClient(self);
}

void ICEC_FUNC_ATTR
Ice_ObjectAdapter_List_init(Ice_ObjectAdapter_ListPtr self) {
    trace();
    self->begin[0] = NULL;
    self->count = 0;

    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    trace();
    Ptr_check(self);

    if (self->connection.fd != -1) {
        return;
    }

    IcePlugin_ObjectPrx_connect(self);
    assert(self->connection.fd != -1);

    if (self->epinfo.datagram)
        return;

    if (self->mode == Ice_ReferenceModeOneway || self->mode == Ice_ReferenceModeTwoway)
        Ice_Connection_receiveValidate(&(self->connection));
}

void ICEC_FUNC_ATTR
Ice_ObjectPrx_closeConnection(Ice_ObjectPrxPtr self) {
    trace();
    Ptr_check(self);

    if (self->epinfo.datagram)
        return;

    if (self->connection.fd == -1) {
        return;
    }

    if (self->mode == Ice_ReferenceModeOneway || self->mode == Ice_ReferenceModeTwoway)
        Ice_Connection_sendClose(&(self->connection));

    IcePlugin_Connection_close(&(self->connection));
    self->connection.fd = -1;
}

void ICEC_FUNC_ATTR
Ice_ObjectPrx_send(Ice_ObjectPrxPtr self) {
    trace();
    Ptr_check(self);

#ifdef ICE_TRACE_CONNECTION
    dissect_message(self->stream.data, self->stream.size);
#endif

    Ice_Connection_send(&(self->connection), self->stream.data, self->stream.size);
}

void ICEC_FUNC_ATTR
Ice_ObjectPrx_readFromInputStream(Ice_ObjectPrxPtr self, Ice_InputStreamPtr is) {
    byte b, facetSeqSize, numOfEndpoints;
    Ice_Short type;

    trace();
    Ptr_check(is);

    Object_init((ObjectPtr)self);
    Ice_InputStream_readIdentity(is, self->identity);

    /* empty proxy, no more fields encoded */
    if (self->identity[0] == 0)
        return;

    /* facet not supported, must be 0 */
    Ice_InputStream_readByte(is, &facetSeqSize);
    assert(facetSeqSize == 0);

    Ice_InputStream_readByte(is, &b); /* mode */
    Ice_InputStream_readByte(is, &b); /* secure */
    Ice_InputStream_readByte(is, &numOfEndpoints);
    assert(numOfEndpoints == 1);

    /* endpoint type (short) */
    Ice_InputStream_readShort(is, &type);
    self->epinfo.type = type;

    /* Endpoint related fields */
    IcePlugin_EndpointInfo_readFromInputStream(&self->epinfo, is);

    /* init internal state */
    Ice_Connection_init(&(self->connection), &(self->epinfo));
    Ice_OutputStream_init(&(self->stream));
}

void ICEC_FUNC_ATTR
Ice_ObjectPrx_writeToOutputStream(Ice_ObjectPrxPtr self, Ice_OutputStreamPtr os) {
    trace();
    Ptr_check(os);

    /* if null proxy, write empty Identity (0 0) and return */
    if (! self) {
        Ice_OutputStream_writeByte(os, 0);
        Ice_OutputStream_writeByte(os, 0);
        return;
    }

    Ptr_check(self);

    Ice_OutputStream_writeIdentity(os, self->identity);   /* identity */
    Ice_OutputStream_writeByte(os, 0);                    /* facet */
    Ice_OutputStream_writeByte(os, self->mode);           /* mode */
    Ice_OutputStream_writeByte(os, 0);                    /* secure */

    Ice_OutputStream_writeByte(os, 1);                    /* num of endpoints */
    Ice_OutputStream_writeShort(os, self->epinfo.type);   /* endpoint type */

    /* Endpoint related fields */
    IcePlugin_EndpointInfo_writeToOutputStream(&self->epinfo, os);
}

void ICEC_FUNC_ATTR
Ice_ObjectPrx_ice_identity(Ice_ObjectPrxPtr self, const char* newid) {
    trace();

    /* 'less than', because needs an extra byte for '\0' */
    assert(strlen(newid) < MAX_IDENTITY_SIZE);

    strcpy(self->identity, newid);
}

void ICEC_FUNC_ATTR
Ice_Communicator_init(Ice_CommunicatorPtr self) {
    trace();
    Ice_ObjectAdapter_List_init(&(self->adapters));
    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
Ice_Communicator_createObjectAdapterWithEndpoints(Ice_CommunicatorPtr self,
                                                  const char* name,
                                                  const char* endpoints,
                                                  Ice_ObjectAdapterPtr result) {
    Ice_EndpointType type;
    (void) name; /* avoid compiler warnings */

    /* print_dbg("adapter eps: '"); */
    /* print_dbg(endpoints); */
    /* print_dbg("'\n") */

    trace();
    Ptr_check(self);

    IceUtil_getProtocolType(endpoints, &type);
    IcePlugin_EndpointInfo_init(&(result->epinfo), type, endpoints);
    Ice_ObjectAdapter_init(result, self);
}

void ICEC_FUNC_ATTR
Ice_Communicator_registerAdapter(Ice_CommunicatorPtr self,
                                 Ice_ObjectAdapterPtr adapter) {
    trace();
    Ptr_check(self);
    Ptr_check(adapter);

    assert(self->adapters.count < MAX_NUM_OF_ADAPTERS);
    self->adapters.begin[(self->adapters.count)++] = adapter;
}

void
Ice_Communicator_stringToProxy(Ice_CommunicatorPtr self, const char* str,
                               Ice_ObjectPrx* result) {
    Ice_EndpointType type;

    trace();
    Ptr_check(self);

    IceUtil_getProtocolType(str, &type);
    IceUtil_getIdentity(str, result->identity);
    IceUtil_getMode(str, &(result->mode));

    IcePlugin_ObjectPrx_init(result, type, str);
}

void ICEC_FUNC_ATTR
Ice_Communicator_waitForShutdown(Ice_CommunicatorPtr self) {
    uint8_t i;
    Ice_ObjectAdapterPtr result[MAX_NUM_OF_ADAPTERS];

    trace();
    Ptr_check(self);

    while (true) {
        IceInternal_selectOnAdapters(&(self->adapters), result, true);

        for (i=0; i<self->adapters.count; i++) {
            if (result[i] == NULL)
                continue;

            Ice_ObjectAdapter_incomingMessage(result[i]);
        }
    }
}

void ICEC_FUNC_ATTR
Ice_Communicator_loopIteration(Ice_CommunicatorPtr self) {
    uint8_t i;
    Ice_ObjectAdapterPtr result[MAX_NUM_OF_ADAPTERS];

    /* trace(); */
    Ptr_check(self);

    IceInternal_selectOnAdapters(&(self->adapters), result, false);

    for (i=0; i<self->adapters.count; i++) {
        if (result[i] == NULL)
            continue;

        Ice_ObjectAdapter_incomingMessage(result[i]);
    }
}

static IcePlugin_PluginListItem _registered_endpoints;

void ICEC_FUNC_ATTR
Ice_initialize(Ice_CommunicatorPtr ic) {
    trace();

    /* print_dbg("initialize"); */

    Ice_Communicator_init(ic);
    IcePlugin_PluginListItem_init(&_registered_endpoints, NULL);
}

/*
  FIXME: IcePlugin, refactor using something like a 'foreach'
  function to iter over plugins
*/

void ICEC_FUNC_ATTR
IcePlugin_PluginListItem_init(IcePlugin_PluginListItemPtr self, ObjectPtr object) {
    trace();
    Object_init((ObjectPtr)self);

    self->object = object;
    self->next = NULL;
}

void ICEC_FUNC_ATTR
IcePlugin_EndpointObject_init(IcePlugin_EndpointObjectPtr self) {
    trace();
    Object_init((ObjectPtr)self);
}

void ICEC_FUNC_ATTR
IcePlugin_registerEndpoint(IcePlugin_PluginListItemPtr item) {
    IcePlugin_PluginListItemPtr last = &_registered_endpoints;

    trace();
    Ptr_check(item);

    while(last->next != NULL)
        last = last->next;

    last->next = item;
}

void ICEC_FUNC_ATTR
IcePlugin_getProtocolType(const char* proto, Ice_EndpointType* result) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->getProtocolType(proto, result))
            break;

        item = item->next;
    }
}

void ICEC_FUNC_ATTR
IcePlugin_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->InputStream_init(self, fd, type))
            break;

        item = item->next;
    }
}

void ICEC_FUNC_ATTR
IcePlugin_Connection_accept(Ice_ConnectionPtr self) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->Connection_accept(self))
            break;

        item = item->next;
    }
}

int ICEC_FUNC_ATTR
IcePlugin_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t size) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;
    uint16_t sent = 0;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        sent = size;

        Ptr_check(endp);
        if (endp->Connection_send(self, fd, data, &sent))
            break;

        sent = 0;
        item = item->next;
    }

    return sent;
}

void ICEC_FUNC_ATTR
IcePlugin_Connection_close(Ice_ConnectionPtr self) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->Connection_close(self))
            break;

        item = item->next;
    }
}

void ICEC_FUNC_ATTR
IcePlugin_Connection_dataReady(Ice_ConnectionPtr self, bool* result) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    /* trace(); */

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->Connection_dataReady(self, result))
            break;

        item = item->next;
    }
}

void ICEC_FUNC_ATTR
IcePlugin_EndpointInfo_init(Ice_EndpointInfoPtr self, Ice_EndpointType type,
                            const char* endpoint) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->EndpointInfo_init(self, type, endpoint))
            break;

        item = item->next;
    }
}

void ICEC_FUNC_ATTR
IcePlugin_EndpointInfo_writeToOutputStream(Ice_EndpointInfoPtr self,
                                           Ice_OutputStreamPtr os) {

    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->EndpointInfo_writeToOutputStream(self, os))
            break;

        item = item->next;
    }
}

void ICEC_FUNC_ATTR
IcePlugin_EndpointInfo_readFromInputStream(Ice_EndpointInfoPtr self,
                                           Ice_InputStreamPtr is) {

    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->EndpointInfo_readFromInputStream(self, is))
            break;

        item = item->next;
    }
}

bool ICEC_FUNC_ATTR
IcePlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->ObjectAdapter_activate(self))
            return true;

        item = item->next;
    }
    return false;
}

void ICEC_FUNC_ATTR
IcePlugin_ObjectPrx_init(Ice_ObjectPrxPtr self,
                         Ice_EndpointType type,
                         const char* strprx) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->ObjectPrx_init(self, type, strprx))
            break;

        item = item->next;
    }
}

void ICEC_FUNC_ATTR
IcePlugin_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
        IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
        Ptr_check(endp);

        if (endp->ObjectPrx_connect(self))
            break;

        item = item->next;
    }
}

/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>

void
Object_init(ObjectPtr self) {
    self->tag = TAG_OK;
}

void
IceUtil_getIdentity(const char* s, char* result) {
    int end;

    trace();
    s += strspn(s, "\n\r\t ");
    end = strcspn(s, "\n\r\t ");
    strncpy(result, s, end);
    result[end] = 0;
}

void
IceUtil_getProtocolType(const char* s, Ice_EndpointType* result) {
    char proto[MAX_PROTO_SIZE];
    const char* i;
    int end;

    trace();
    proto[0] = 0;

    s += strspn(s, "\n\r\t ");

    i = index(s, ':');
    if (i != NULL) {
	i++;
	s = i + strspn(i, "\n\r\t ");
    }

    end = strcspn(s, "\n\r\t ");
    strncpy(proto, s, end);
    proto[end] = 0;

    *result = 0;
    IcePlugin_getProtocolType(proto, result);

    if (*result == 0)
	assert(false && "Invalid endpoint type!");
}

void
IceUtil_getMode(const char* s, Ice_ReferenceMode* mode) {
    ssize_t i, j;
    const char* t = " -";
    ssize_t token_size = 2;
    int limit = strchr(s, ':') - s;

    for (i=0; i<limit; i++) {
	for (j=0; j<token_size; j++) {
	    if (s[i+j] != t[j])
		break;
	    if ((j+1) == token_size && (i+j+1) < limit) {
		switch(s[i+j+1]) {
		case 'd': *mode = Ice_ReferenceModeDatagram; return;
		case 'o': *mode = Ice_ReferenceModeOneway; return;
		case 't': *mode = Ice_ReferenceModeTwoway; return;
		}
	    }
	}
    }

    throw("Ice_InvalidEndpointException", __FILE__, __LINE__);
}

void
Ice_Object_init(Ice_ObjectPtr self, Ice_MethodHandlerPtr handler) {
    trace();
    self->handler = handler;
    Object_init((ObjectPtr)self);
}

void
Ice_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) {
    trace();

    IcePlugin_InputStream_init(self, fd, type);

    self->next = self->data;
    Object_init((ObjectPtr)self);
}

void
Ice_InputStream_readBool(Ice_InputStreamPtr self, Ice_Bool* v) {
    trace();
    Ptr_check(self);

    *v = *(self->next++) != 0;
}

void
Ice_InputStream_readByte(Ice_InputStreamPtr self, Ice_Byte* v) {
    trace();
    Ptr_check(self);

    *v = *(self->next++);
}

void
Ice_InputStream_readShort(Ice_InputStreamPtr self, Ice_Short* v) {
    Ice_Byte* src;
    Ice_Byte* dest;

    trace();
    Ptr_check(self);

    if (self->next - self->data + sizeof(Ice_Short) > (unsigned int)self->size) {
	throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
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

void
Ice_InputStream_readInt(Ice_InputStreamPtr self, Ice_Int* v) {
    Ice_Byte* src;
    Ice_Byte* dest;

    trace();
    Ptr_check(self);

    if (self->next - self->data + sizeof(Ice_Int) > (unsigned int)self->size) {
	throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
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

void
Ice_InputStream_readSize(Ice_InputStreamPtr self, Ice_Int* size, bool increment) {
    Ice_Byte b;

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

void
Ice_InputStream_readString(Ice_InputStreamPtr self, char* v) {
    Ice_Int size;

    trace();
    Ptr_check(self);

    Ice_InputStream_readSize(self, &size, true);
    if (v != NULL)  {
	strncpy(v, (char*)self->next, size);
	v[size] = 0;
    }
    self->next += size;
}

void
Ice_InputStream_readBlob(Ice_InputStreamPtr self, byte* v, int size) {
    trace();
    Ptr_check(self);

    assert(self->size - (self->next - self->data) >= size);

    memcpy(v, self->next, size);
    self->next += size;
}

void
Ice_InputStream_readIdentity(Ice_InputStreamPtr self, char* name) {
    trace();
    Ptr_check(self);

    Ice_InputStream_readString(self, name); /* name */
    Ice_InputStream_readString(self, NULL); /* category (not supported) */
}

void
Ice_OutputStream_init(Ice_OutputStreamPtr self) {
    trace();
    self->size = 0;
    self->next = self->data;
    self->encaps[0] = NULL;
    self->current_encaps = 0;

    Object_init((ObjectPtr)self);
}

void
Ice_OutputStream_writeBool(Ice_OutputStreamPtr self, Ice_Bool v) {
    trace();
    Ptr_check(self);

    assert((self->size + 1 <= MAX_MESSAGE_SIZE));

    *(self->next++) = v ? 1 : 0;
    self->size++;
}

void
Ice_OutputStream_writeByte(Ice_OutputStreamPtr self, Ice_Byte v) {
    trace();
    Ptr_check(self);

    assert((self->size + 1 <= MAX_MESSAGE_SIZE));

    *(self->next++) = v;
    self->size++;
}

void
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

void
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

void
Ice_OutputStream_writeBlob(Ice_OutputStreamPtr self, const byte* data, int size) {
    trace();
    Ptr_check(self);

    assert((self->size + size <= MAX_MESSAGE_SIZE) && "Too long message!");

    memcpy(self->next, data, size);
    self->next += size;
    self->size += size;
}

void
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

void
Ice_OutputStream_writeString(Ice_OutputStreamPtr self, const char* str) {
    int size;

    trace();
    Ptr_check(self);

    size = strlen(str);
    Ice_OutputStream_writeSize(self, size);
    Ice_OutputStream_writeBlob(self, (byte*)str, size);
}

void
Ice_OutputStream_writeIdentity(Ice_OutputStreamPtr self, const char* id) {
    trace();
    Ptr_check(self);

    if (strcspn(id, "/") != strlen(id)) {
	fixme("category not supported");
    }

    Ice_OutputStream_writeString(self, id); /* name */
    Ice_OutputStream_writeByte(self, 0);    /* category */
}

void
Ice_OutputStream_startWriteEncaps(Ice_OutputStreamPtr self) {
    trace();
    Ptr_check(self);

    assert(self->current_encaps + 1 < MAX_NESTED_ENCAPS);
    self->encaps[++(self->current_encaps)] = self->next;

    Ice_OutputStream_writeInt(self, 0); /* placeholder for encapsulation length */
    Ice_OutputStream_writeByte(self, encodingMajor);
    Ice_OutputStream_writeByte(self, encodingMinor);
}

void
Ice_OutputStream_endWriteEncaps(Ice_OutputStreamPtr self) {
    Ice_Int* size;

    trace();
    Ptr_check(self);

    size = (Ice_Int*)self->encaps[self->current_encaps];
    *size = self->next - self->encaps[self->current_encaps];

    self->encaps[(self->current_encaps)--] = NULL;
}

void
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

void
Ice_Connection_init(Ice_ConnectionPtr self, Ice_EndpointType type) {
    trace();

    self->fd = -1;
    self->current_client = -1;
    self->type = type;
    Object_init((ObjectPtr)self);
}

void
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

    if (res == 0) {
	error_at_line(0, 0, __FILE__, __LINE__, "Connection lost!\n");
	return;
    }

    if (res == -1) {
	error_at_line(0, errno, __FILE__, __LINE__, "Error on socket");
	return;
    }

    error_at_line(0, 0, __FILE__, __LINE__, "Could not write all bytes on socket!\n");
}

void
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

void
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

    Ice_InputStream_init(&is, self->fd, self->type);

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(is.data, is.size);
#endif

    if (memcmp(expected, is.next, 14) != 0) {
	throw("Ice_ProtocolException", __FILE__, __LINE__);
	return;
    }
}

void
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

void
Ice_ServantMap_init(Ice_ServantMapPtr self) {
    trace();
    self->count = 0;
    Object_init((ObjectPtr)self);
}

void
Ice_ObjectAdapter_init(Ice_ObjectAdapterPtr self,
		       Ice_CommunicatorPtr communicator) {

    trace();
    Ptr_check(communicator);

    self->communicator = communicator;
    self->activated = false;

    Ice_Connection_init(&(self->connection), self->epinfo.type);
    Ice_ServantMap_init(&(self->servantMap));
    Object_init((ObjectPtr)self);
}

void
Ice_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    bool ok;

    trace();
    Ptr_check(self);

    ok = IcePlugin_ObjectAdapter_activate(self);
    if (! ok) {
	throw("Ice_NoEndpointException", __FILE__, __LINE__);
	return;
    }

    Ice_Communicator_registerAdapter(self->communicator, self);
    self->activated = true;
}

void
Ice_ObjectAdapter_add(Ice_ObjectAdapterPtr self,
		      Ice_ObjectPtr servant,
		      const char* identity) {
    Ice_ServantMapPtr sm;

    trace();
    Ptr_check(self);
    Ptr_check(servant);
    assert(strlen(identity) < MAX_IDENTITY_SIZE);

    sm = &(self->servantMap);
    strcpy(sm->ids[sm->count], identity);
    sm->objects[sm->count] = servant;
    sm->count++;
}

int
Ice_ObjectAdapter_getClient(Ice_ObjectAdapterPtr self) {
    trace();
    Ptr_check(self);

    if (! self->epinfo.datagram) {
	IcePlugin_Connection_accept(&(self->connection));
	Ice_Connection_sendValidate(&(self->connection));
	return self->connection.current_client;
    }

    return self->connection.fd;
}

int
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

void
Ice_ObjectAdapter_incomingMessage(Ice_ObjectAdapterPtr self) {
    int fd, i;
    Ice_Int size, requestId;
    Ice_InputStream is;
    char identity[MAX_IDENTITY_SIZE];
    byte facetSeqSize;

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

    fd = Ice_ObjectAdapter_getClient(self);
    Ice_InputStream_init(&is, fd, self->connection.type);

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(is.data, is.size);
#endif

    if (memcmp(header, is.next, 10) != 0) {
	throw("Ice_ProtocolException", __FILE__, __LINE__);
	Ice_ObjectAdapter_closeClient(self);
	return;
    }
    is.next += 10;

    Ice_InputStream_readInt(&is, &size);

    if (size > MAX_MESSAGE_SIZE) {
	throw("Ice_MemoryLimitException", __FILE__, __LINE__);
	Ice_ObjectAdapter_closeClient(self);
	return;
    }

    Ice_InputStream_readInt(&is, &requestId);
    Ice_InputStream_readIdentity(&is, identity);

    /* facet not supported */
    Ice_InputStream_readByte(&is, &facetSeqSize);
    assert(facetSeqSize == 0);

    for (i=0; i<self->servantMap.count; i++) {
	if (streq(self->servantMap.ids[i], identity)) {
	    Ice_OutputStream os;

	    Ice_ObjectPtr servant = self->servantMap.objects[i];
	    assert(servant->handler != NULL);

	    /* prepare reply message */
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

	    if (requestId > 0) {
		Ice_OutputStream_endWriteEncaps(&os);
		Ice_OutputStream_setMessageSize(&os);
		Ice_Connection_send(&(self->connection), os.data, os.size);
	    }

	    Ice_ObjectAdapter_closeClient(self);
	    return;
	}
    }

    throw("Ice_ObjectNotExistException", __FILE__, __LINE__);
    Ice_ObjectAdapter_closeClient(self);
}

void
Ice_ObjectAdapter_List_init(Ice_ObjectAdapter_ListPtr self) {
    trace();
    self->begin[0] = NULL;
    self->count = 0;

    Object_init((ObjectPtr)self);
}

void
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

void
Ice_ObjectPrx_closeConnection(Ice_ObjectPrxPtr self) {
    trace();
    Ptr_check(self);

    if (self->epinfo.datagram)
	return;

    if (self->mode == Ice_ReferenceModeOneway || self->mode == Ice_ReferenceModeTwoway)
	Ice_Connection_sendClose(&(self->connection));
}

void
Ice_ObjectPrx_send(Ice_ObjectPrxPtr self) {
    trace();
    Ptr_check(self);

#ifdef ICE_TRACE_CONNECTION
    dissect_message(self->stream.data, self->stream.size);
#endif

    Ice_Connection_send(&(self->connection), self->stream.data, self->stream.size);
}

void
Ice_ObjectPrx_readFromInputStream(Ice_ObjectPrxPtr self, Ice_InputStreamPtr is) {
    char identity[MAX_IDENTITY_SIZE];
    byte b, facetSeqSize, numOfEndpoints;

    trace();
    Ptr_check(is);

    Ice_InputStream_readIdentity(is, identity);
    Ice_InputStream_readByte(is, &facetSeqSize);
    assert(facetSeqSize == 0);

    Ice_InputStream_readByte(is, &b); /* mode */
    Ice_InputStream_readByte(is, &b); /* secure */
    Ice_InputStream_readByte(is, &numOfEndpoints);
    assert(numOfEndpoints == 1);

    /* FIXME: read endpoint type and call IcePlugin_ObjectPrx_readFromInputStream */
    fixme("Not finished implementation");
}

void
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

    /* FIXME: call IcePlugin_ObjectPrx_writeToOutputStream */
    fixme("Not finished implementation");
}


void
Ice_Communicator_init(Ice_CommunicatorPtr self) {
    trace();
    Ice_ObjectAdapter_List_init(&(self->adapters));
    Object_init((ObjectPtr)self);
}

void
Ice_Communicator_createObjectAdapterWithEndpoints(Ice_CommunicatorPtr self,
						  const char* name,
						  const char* endpoints,
						  Ice_ObjectAdapterPtr result) {
    Ice_EndpointType type;
    (void) name; /* avoid compiler warnings */

    trace();
    Ptr_check(self);

    IceUtil_getProtocolType(endpoints, &type);
    IcePlugin_EndpointInfo_init(&(result->epinfo), type, endpoints);
    Ice_ObjectAdapter_init(result, self);
}

void
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

void
Ice_Communicator_waitForShutdown(Ice_CommunicatorPtr self) {

    uint8_t i;
    Ice_ObjectAdapterPtr result[MAX_NUM_OF_ADAPTERS];

    trace();
    Ptr_check(self);

    while (true) {
	IceInternal_selectOnAdapters(&(self->adapters), result);

	for (i=0; i<self->adapters.count; i++) {
	    if (result[i] == NULL)
		continue;

	    Ice_ObjectAdapter_incomingMessage(result[i]);
	}
    }
}

static IcePlugin_PluginListItem _registered_endpoints;

void
Ice_initialize(Ice_CommunicatorPtr ic) {
    trace();
    Ice_Communicator_init(ic);
    IcePlugin_PluginListItem_init(&_registered_endpoints, NULL);
}

/*
  FIXME: IcePlugin, refactor using something like a 'foreach'
  function to iter over plugins
*/

void
IcePlugin_PluginListItem_init(IcePlugin_PluginListItemPtr self, ObjectPtr object) {
    trace();
    Object_init((ObjectPtr)self);

    self->object = object;
    self->next = NULL;
}

void
IcePlugin_EndpointObject_init(IcePlugin_EndpointObjectPtr self) {
    trace();
    Object_init((ObjectPtr)self);
}

void
IcePlugin_registerEndpoint(IcePlugin_PluginListItemPtr item) {
    IcePlugin_PluginListItemPtr last = &_registered_endpoints;

    trace();
    Ptr_check(item);

    while(last->next != NULL)
	last = last->next;

    last->next = item;
}

void
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

void
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

void
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

int
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

void
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

void
IcePlugin_Connection_dataReady(Ice_ConnectionPtr self, bool* result) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->Connection_dataReady(self, result))
    	    break;

    	item = item->next;
    }
}

void
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

bool
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

void
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

void
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

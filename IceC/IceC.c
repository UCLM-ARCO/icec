/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>

void
Object_init(ObjectPtr this) {
    this->tag = TAG_OK;
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
Ice_Object_init(Ice_ObjectPtr this, Ice_MethodHandlerPtr handler) {
    trace();
    this->handler = handler;
    Object_init((ObjectPtr)this);
}

void
Ice_InputStream_init(Ice_InputStreamPtr this, int fd, Ice_EndpointType type) {
    trace();

    IcePlugin_InputStream_init(this, fd, type);

    this->next = this->data;
    Object_init((ObjectPtr)this);
}

void
Ice_InputStream_readBool(Ice_InputStreamPtr this, Ice_Bool* v) {
    trace();
    Ptr_check(this);

    *v = *(this->next++) != 0;
}

void
Ice_InputStream_readByte(Ice_InputStreamPtr this, Ice_Byte* v) {
    trace();
    Ptr_check(this);

    *v = *(this->next++);
}

void
Ice_InputStream_readShort(Ice_InputStreamPtr this, Ice_Short* v) {
    Ice_Byte* src;
    Ice_Byte* dest;

    trace();
    Ptr_check(this);

    if (this->next - this->data + sizeof(Ice_Short) > (unsigned int)this->size) {
	throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
	return;
    }

    src = this->next;
    this->next += sizeof(Ice_Short);

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
Ice_InputStream_readInt(Ice_InputStreamPtr this, Ice_Int* v) {
    Ice_Byte* src;
    Ice_Byte* dest;

    trace();
    Ptr_check(this);

    if (this->next - this->data + sizeof(Ice_Int) > (unsigned int)this->size) {
	throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
	return;
    }

    src = this->next;
    this->next += sizeof(Ice_Int);

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
Ice_InputStream_readSize(Ice_InputStreamPtr this, Ice_Int* size, bool increment) {
    Ice_Byte b;

    trace();
    Ptr_check(this);

    Ice_InputStream_readByte(this, &b);

    if (b < 255) {
	*size = b;
	if (! increment) this->next--;
	return;
    }

    Ice_InputStream_readInt(this, size);
    if (! increment) this->next -= sizeof(Ice_Int) + 1;
    assert(*size >= 0);
}

void
Ice_InputStream_readString(Ice_InputStreamPtr this, char* v) {
    Ice_Int size;

    trace();
    Ptr_check(this);

    Ice_InputStream_readSize(this, &size, true);
    if (v != NULL)  {
	strncpy(v, (char*)this->next, size);
	v[size] = 0;
    }
    this->next += size;
}

void
Ice_InputStream_readBlob(Ice_InputStreamPtr this, byte* v, int size) {
    trace();
    Ptr_check(this);

    assert(this->size - (this->next - this->data) >= size);

    memcpy(v, this->next, size);
    this->next += size;
}

void
Ice_InputStream_readIdentity(Ice_InputStreamPtr this, char* name) {
    trace();
    Ptr_check(this);

    Ice_InputStream_readString(this, name); /* name */
    Ice_InputStream_readString(this, NULL); /* category (not supported) */
}

void
Ice_OutputStream_init(Ice_OutputStreamPtr this) {
    trace();
    this->size = 0;
    this->next = this->data;
    this->encaps[0] = NULL;
    this->current_encaps = 0;

    Object_init((ObjectPtr)this);
}

void
Ice_OutputStream_writeBool(Ice_OutputStreamPtr this, Ice_Bool v) {
    trace();
    Ptr_check(this);

    assert((this->size + 1 <= MAX_MESSAGE_SIZE));

    *(this->next++) = v ? 1 : 0;
    this->size++;
}

void
Ice_OutputStream_writeByte(Ice_OutputStreamPtr this, Ice_Byte v) {
    trace();
    Ptr_check(this);

    assert((this->size + 1 <= MAX_MESSAGE_SIZE));

    *(this->next++) = v;
    this->size++;
}

void
Ice_OutputStream_writeShort(Ice_OutputStreamPtr this, Ice_Short v) {
    byte size;
    byte* dest;
    byte* src;

    trace();
    Ptr_check(this);

    size = sizeof(Ice_Short);
    assert((this->size + size <= MAX_MESSAGE_SIZE));

    dest = this->next;

#ifdef ICE_BIG_ENDIAN
    src = (byte*)&v + sizeof(Ice_Short) - 1;
    *dest++ = *src--;
    *dest = *src;
#else
    src = (byte*)&v;
    *dest++ = *src++;
    *dest = *src;
#endif

    this->next += size;
    this->size += size;
}

void
Ice_OutputStream_writeInt(Ice_OutputStreamPtr this, Ice_Int v) {
    byte size;
    byte* dest;
    byte* src;

    trace();
    Ptr_check(this);

    size = sizeof(Ice_Int);
    assert((this->size + size <= MAX_MESSAGE_SIZE) && "Too long message!");

    dest = this->next;

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

    this->next += size;
    this->size += size;
}

void
Ice_OutputStream_writeBlob(Ice_OutputStreamPtr this, const byte* data, int size) {
    trace();
    Ptr_check(this);

    assert((this->size + size <= MAX_MESSAGE_SIZE) && "Too long message!");

    memcpy(this->next, data, size);
    this->next += size;
    this->size += size;
}

void
Ice_OutputStream_writeSize(Ice_OutputStreamPtr this, int size) {
    trace();
    Ptr_check(this);

    assert (size >= 0);
    if (size > 254) {
	Ice_OutputStream_writeByte(this, 255);
	Ice_OutputStream_writeInt(this, size);
    }
    else {
	Ice_OutputStream_writeByte(this, (byte)size);
    }
}

void
Ice_OutputStream_writeString(Ice_OutputStreamPtr this, const char* str) {
    int size;

    trace();
    Ptr_check(this);

    size = strlen(str);
    Ice_OutputStream_writeSize(this, size);
    Ice_OutputStream_writeBlob(this, (byte*)str, size);
}

void
Ice_OutputStream_writeIdentity(Ice_OutputStreamPtr this, const char* id) {
    trace();
    Ptr_check(this);

    if (strcspn(id, "/") != strlen(id)) {
	fixme("category not supported");
    }

    Ice_OutputStream_writeString(this, id); /* name */
    Ice_OutputStream_writeByte(this, 0);    /* category */
}

void
Ice_OutputStream_startWriteEncaps(Ice_OutputStreamPtr this) {
    trace();
    Ptr_check(this);

    assert(this->current_encaps + 1 < MAX_NESTED_ENCAPS);
    this->encaps[++(this->current_encaps)] = this->next;

    Ice_OutputStream_writeInt(this, 0); /* placeholder for encapsulation length */
    Ice_OutputStream_writeByte(this, encodingMajor);
    Ice_OutputStream_writeByte(this, encodingMinor);
}

void
Ice_OutputStream_endWriteEncaps(Ice_OutputStreamPtr this) {
    Ice_Int* size;

    trace();
    Ptr_check(this);

    size = (Ice_Int*)this->encaps[this->current_encaps];
    *size = this->next - this->encaps[this->current_encaps];

    this->encaps[(this->current_encaps)--] = NULL;
}

void
Ice_OutputStream_setMessageSize(Ice_OutputStreamPtr this) {
    byte* next;
    int size;

    trace();
    Ptr_check(this);

    next = this->next;
    size = this->size;

    /* Ice header is fixed length, size starts at position 11 */
    this->next = this->data + 10;
    Ice_OutputStream_writeInt(this, size);

    this->next = next;
    this->size = size;
}

void
Ice_Connection_init(Ice_ConnectionPtr this, Ice_EndpointType type) {
    trace();

    this->fd = -1;
    this->current_client = -1;
    this->type = type;
    Object_init((ObjectPtr)this);
}

void
Ice_Connection_send(Ice_ConnectionPtr this, byte* data, int size) {
    int fd, res;

    trace();
    Ptr_check(this);

    fd = this->current_client;
    if (fd == -1) fd = this->fd;

    assert(this->fd > -1);

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(data, size);
#endif

    res = IcePlugin_Connection_send(this, fd, data, size);
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
Ice_Connection_sendValidate(Ice_ConnectionPtr this) {
    Ice_OutputStream os;

    trace();
    Ptr_check(this);

    Ice_OutputStream_init(&os);
    Ice_OutputStream_writeBlob(&os, Ice_header, 8);
    Ice_OutputStream_writeByte(&os, Ice_validateConnectionMsg);
    Ice_OutputStream_writeByte(&os, 0);  /* compression */
    Ice_OutputStream_writeInt(&os, 14);

    Ice_Connection_send(this, os.data, os.size);
}

void
Ice_Connection_receiveValidate(Ice_ConnectionPtr this) {
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
    Ptr_check(this);

    Ice_InputStream_init(&is, this->fd, this->type);

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(is.data, is.size);
#endif

    if (memcmp(expected, is.next, 14) != 0) {
	throw("Ice_ProtocolException", __FILE__, __LINE__);
	return;
    }
}

void
Ice_Connection_sendClose(Ice_ConnectionPtr this) {
    Ice_OutputStream os;

    trace();
    Ptr_check(this);

    Ice_OutputStream_init(&os);
    Ice_OutputStream_writeBlob(&os, Ice_header, 8);
    Ice_OutputStream_writeByte(&os, Ice_closeConnectionMsg);
    Ice_OutputStream_writeByte(&os, 0);  /* compression */
    Ice_OutputStream_writeInt(&os, 14);

    Ice_Connection_send(this, os.data, os.size);
}

void
Ice_ServantMap_init(Ice_ServantMapPtr this) {
    trace();
    this->count = 0;
    Object_init((ObjectPtr)this);
}

void
Ice_ObjectAdapter_init(Ice_ObjectAdapterPtr this,
		       Ice_CommunicatorPtr communicator) {

    trace();
    Ptr_check(communicator);

    this->communicator = communicator;
    this->activated = false;

    Ice_Connection_init(&(this->connection), this->epinfo.type);
    Ice_ServantMap_init(&(this->servantMap));
    Object_init((ObjectPtr)this);
}

void
Ice_ObjectAdapter_activate(Ice_ObjectAdapterPtr this) {
    bool ok;

    trace();
    Ptr_check(this);

    ok = IcePlugin_ObjectAdapter_activate(this);
    if (! ok) {
	throw("Ice_NoEndpointException", __FILE__, __LINE__);
	return;
    }

    Ice_Communicator_registerAdapter(this->communicator, this);
    this->activated = true;
}

void
Ice_ObjectAdapter_add(Ice_ObjectAdapterPtr this,
		      Ice_ObjectPtr servant,
		      const char* identity) {
    Ice_ServantMapPtr sm;

    trace();
    Ptr_check(this);
    Ptr_check(servant);
    assert(strlen(identity) < MAX_IDENTITY_SIZE);

    sm = &(this->servantMap);
    strcpy(sm->ids[sm->count], identity);
    sm->objects[sm->count] = servant;
    sm->count++;
}

int
Ice_ObjectAdapter_getClient(Ice_ObjectAdapterPtr this) {
    trace();
    Ptr_check(this);

    if (! this->epinfo.datagram) {
	IcePlugin_Connection_accept(&(this->connection));
	Ice_Connection_sendValidate(&(this->connection));
	return this->connection.current_client;
    }

    return this->connection.fd;
}

int
Ice_ObjectAdapter_closeClient(Ice_ObjectAdapterPtr this) {
    trace();
    Ptr_check(this);

    if (! this->epinfo.datagram) {
	Ice_Connection_sendClose(&(this->connection));
	IcePlugin_Connection_close(&(this->connection));
	this->connection.current_client = -1;
    }

    return 0;
}

void
Ice_ObjectAdapter_incomingMessage(Ice_ObjectAdapterPtr this) {
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
    Ptr_check(this);

    fd = Ice_ObjectAdapter_getClient(this);
    Ice_InputStream_init(&is, fd, this->connection.type);

#ifdef ICE_TRACE_PROTOCOL
    dissect_message(is.data, is.size);
#endif

    if (memcmp(header, is.next, 10) != 0) {
	throw("Ice_ProtocolException", __FILE__, __LINE__);
	Ice_ObjectAdapter_closeClient(this);
	return;
    }
    is.next += 10;

    Ice_InputStream_readInt(&is, &size);
    if (size > MAX_MESSAGE_SIZE) {
	throw("Ice_MemoryLimitException", __FILE__, __LINE__);
	Ice_ObjectAdapter_closeClient(this);
	return;
    }

    Ice_InputStream_readInt(&is, &requestId);
    Ice_InputStream_readIdentity(&is, identity);

    /* facet not supported */
    Ice_InputStream_readByte(&is, &facetSeqSize);
    assert(facetSeqSize == 0);

    for (i=0; i<this->servantMap.count; i++) {
	if (streq(this->servantMap.ids[i], identity)) {
	    Ice_OutputStream os;

	    Ice_ObjectPtr servant = this->servantMap.objects[i];
	    assert(servant->handler != NULL);

	    /* prepare reply message */
	    Ice_OutputStream_init(&os);

	    /* reply header (if twoway) */
	    if (requestId > 0) {
		Ice_OutputStream_writeBlob(&os, Ice_header, 8);
		Ice_OutputStream_writeByte(&os, Ice_replyMsg);
		Ice_OutputStream_writeByte(&os, 0);  /* compression */
		Ice_OutputStream_writeInt(&os, 0);  /* size (placeholder) */

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
		Ice_Connection_send(&(this->connection), os.data, os.size);
	    }

	    Ice_ObjectAdapter_closeClient(this);
	    return;
	}
    }

    throw("Ice_ObjectNotExistException", __FILE__, __LINE__);
    Ice_ObjectAdapter_closeClient(this);
}

void
Ice_ObjectAdapter_List_init(Ice_ObjectAdapter_ListPtr this) {
    trace();
    this->begin[0] = NULL;
    this->count = 0;

    Object_init((ObjectPtr)this);
}

void
Ice_ObjectPrx_connect(Ice_ObjectPrxPtr this) {
    trace();
    Ptr_check(this);

    if (this->connection.fd != -1) {
	return;
    }

    IcePlugin_ObjectPrx_connect(this);
    assert(this->connection.fd != -1);

    if (this->epinfo.datagram)
	return;

    if (this->mode == Ice_ReferenceModeOneway || this->mode == Ice_ReferenceModeTwoway)
	Ice_Connection_receiveValidate(&(this->connection));
}

void
Ice_ObjectPrx_closeConnection(Ice_ObjectPrxPtr this) {
    trace();
    Ptr_check(this);

    if (this->epinfo.datagram)
	return;

    if (this->mode == Ice_ReferenceModeOneway || this->mode == Ice_ReferenceModeTwoway)
	Ice_Connection_sendClose(&(this->connection));
}

void
Ice_ObjectPrx_send(Ice_ObjectPrxPtr this) {
    trace();
    Ptr_check(this);

#ifdef ICE_TRACE_CONNECTION
    dissect_message(this->stream.data, this->stream.size);
#endif

    Ice_Connection_send(&(this->connection), this->stream.data, this->stream.size);
}

void
Ice_ObjectPrx_readFromInputStream(Ice_ObjectPrxPtr this, Ice_InputStreamPtr is) {
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
Ice_ObjectPrx_writeToOutputStream(Ice_ObjectPrxPtr this, Ice_OutputStreamPtr os) {
    trace();
    Ptr_check(os);

    /* if null proxy, write empty Identity (0 0) and return */
    if (! this) {
	Ice_OutputStream_writeByte(os, 0);
	Ice_OutputStream_writeByte(os, 0);
	return;
    }

    Ptr_check(this);

    Ice_OutputStream_writeIdentity(os, this->identity);   /* identity */
    Ice_OutputStream_writeByte(os, 0);                    /* facet */
    Ice_OutputStream_writeByte(os, this->mode);           /* mode */
    Ice_OutputStream_writeByte(os, 0);                    /* secure */

    Ice_OutputStream_writeByte(os, 1);                    /* num of endpoints */
    Ice_OutputStream_writeShort(os, this->epinfo.type);   /* endpoint type */

    /* FIXME: call IcePlugin_ObjectPrx_writeToOutputStream */
    fixme("Not finished implementation");
}


void
Ice_Communicator_init(Ice_CommunicatorPtr this) {
    trace();
    Ice_ObjectAdapter_List_init(&(this->adapters));
    Object_init((ObjectPtr)this);
}

void
Ice_Communicator_createObjectAdapterWithEndpoints(Ice_CommunicatorPtr this,
						  const char* name,
						  const char* endpoints,
						  Ice_ObjectAdapterPtr result) {
    Ice_EndpointType type;
    (void) name; /* avoid compiler warnings */

    trace();
    Ptr_check(this);

    IceUtil_getProtocolType(endpoints, &type);
    IcePlugin_EndpointInfo_init(&(result->epinfo), type, endpoints);
    Ice_ObjectAdapter_init(result, this);
}

void
Ice_Communicator_registerAdapter(Ice_CommunicatorPtr this,
				 Ice_ObjectAdapterPtr adapter) {
    trace();
    Ptr_check(this);
    Ptr_check(adapter);

    assert(this->adapters.count < MAX_NUM_OF_ADAPTERS);
    this->adapters.begin[(this->adapters.count)++] = adapter;
}

void
Ice_Communicator_stringToProxy(Ice_CommunicatorPtr this, const char* str,
			       Ice_ObjectPrx* result) {
    Ice_EndpointType type;

    trace();
    Ptr_check(this);

    IceUtil_getProtocolType(str, &type);
    IceUtil_getIdentity(str, result->identity);
    IceUtil_getMode(str, &(result->mode));

    IcePlugin_ObjectPrx_init(result, type, str);
}

void
Ice_Communicator_waitForShutdown(Ice_CommunicatorPtr this) {

#if defined(ARDUINO)
    throw("Unsupported operation for this platform!", __FILE__, __LINE__);
    return;
#else

    int i, count;
    fd_set read_set;
    Ice_ObjectAdapterPtr adapter;

    int fds[MAX_NUM_OF_ADAPTERS];
    Ice_ObjectAdapterPtr adapters[MAX_NUM_OF_ADAPTERS];

    trace();
    Ptr_check(this);

    while (true) {
    	FD_ZERO(&read_set);

    	count = 0;
    	for (i=0; i<this->adapters.count; i++) {
    	    adapter = *(this->adapters.begin + i);
    	    if (! adapter->activated) {
    		continue;
    	    }

    	    FD_SET(adapter->connection.fd, &read_set);
    	    fds[count] = adapter->connection.fd;
    	    adapters[count] = adapter;
    	    count++;
    	}

    	if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
    	    error("Error while reading from descriptors: %s\n", strerror(errno));
    	    return;
    	}

    	for (i=0; i<count; i++) {
    	    if (! FD_ISSET(fds[i], &read_set)) {
    		continue;
    	    }

    	    Ice_ObjectAdapter_incomingMessage(adapters[i]);
    	}
    }

#endif
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
IcePlugin_PluginListItem_init(IcePlugin_PluginListItemPtr this, ObjectPtr object) {
    trace();
    Object_init((ObjectPtr)this);

    this->object = object;
    this->next = NULL;
}

void
IcePlugin_EndpointObject_init(IcePlugin_EndpointObjectPtr this) {
    trace();
    Object_init((ObjectPtr)this);
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
IcePlugin_InputStream_init(Ice_InputStreamPtr this, int fd, Ice_EndpointType type) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->InputStream_init(this, fd, type))
    	    break;

    	item = item->next;
    }
}

void
IcePlugin_Connection_accept(Ice_ConnectionPtr this) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->Connection_accept(this))
    	    break;

    	item = item->next;
    }
}

int
IcePlugin_Connection_send(Ice_ConnectionPtr this, int fd, byte* data, uint16_t size) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;
    uint16_t sent = 0;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
	sent = size;

    	Ptr_check(endp);
    	if (endp->Connection_send(this, fd, data, &sent))
    	    break;

	sent = 0;
    	item = item->next;
    }

    return sent;
}

void
IcePlugin_Connection_close(Ice_ConnectionPtr this) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->Connection_close(this))
    	    break;

    	item = item->next;
    }
}

void
IcePlugin_EndpointInfo_init(Ice_EndpointInfoPtr this, Ice_EndpointType type,
			    const char* endpoint) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->EndpointInfo_init(this, type, endpoint))
    	    break;

    	item = item->next;
    }
}

bool
IcePlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr this) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->ObjectAdapter_activate(this))
    	    return true;

    	item = item->next;
    }
    return false;
}

void
IcePlugin_ObjectPrx_init(Ice_ObjectPrxPtr this,
			 Ice_EndpointType type,
			 const char* strprx) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->ObjectPrx_init(this, type, strprx))
    	    break;

    	item = item->next;
    }
}

void
IcePlugin_ObjectPrx_connect(Ice_ObjectPrxPtr this) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->ObjectPrx_connect(this))
    	    break;

    	item = item->next;
    }
}

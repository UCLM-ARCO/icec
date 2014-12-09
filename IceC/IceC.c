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
    char* i;
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
    if (streq(proto, "tcp"))
	*result = Ice_EndpointTypeTCP;
    else if (streq(proto, "udp"))
	*result = Ice_EndpointTypeUDP;
    else
	IcePlugin_getProtocolType(proto, result);

    if (*result == 0)
	assert(false && "Invalid endpoint type!");
}

void
IceUtil_getHost(const char* s, char* result) {
    int end;

    trace();
    result[0] = 0;

    while (true) {
	s = index(s, '-');
	if (s == NULL) return;
	if (s[1] == 'h') {
	    s += 2;  /* '-h' */
	    s += strspn(s, "\n\r\t ");
	    end = strcspn(s, "\n\r\t ");
	    strncpy(result, s, end);
	    result[end] = 0;
	    return;
	}
	s++;
    }
}

void
IceUtil_getPort(const char* s, int* result) {
    char strport[10];
    int end;

    trace();
    *result = -1;

    while (true) {
	s = index(s, '-');
	if (s == NULL) return;
	if (s[1] == 'p') {
	    s += 2; /* '-p' */
	    s += strspn(s, "\n\r\t ");
	    end = strcspn(s, "\n\r\t ");
	    strncpy(strport, s, end);
	    strport[end] = 0;

	    *result = atoi(strport);
	    return;
	}
	s++;
    }
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

    switch (type) {
    case Ice_EndpointTypeTCP:
    case Ice_EndpointTypeUDP: {
	this->size = read(fd, (byte*)this->data, MAX_MESSAGE_SIZE);
	break;
    }
    default: {
	Ice_Int message_size = 0;
	this->size = IcePlugin_read(type, fd, (byte*)this->data, 14);
	message_size = (Ice_Int)*(this->data + 10);
	assert(message_size <= MAX_MESSAGE_SIZE);
	this->size += IcePlugin_read(type, fd, (byte*)this->data + 14, message_size - 14);
	break;
    }
    }

    dissect_message((byte*)this->data, this->size);

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
    int size;

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

    switch (this->type) {
    case Ice_EndpointTypeTCP:
    case Ice_EndpointTypeUDP: {
	res = send(fd, data, size, 0);
	break;
    }
    default:
	res = IcePlugin_write(this->type, fd, data, size);
    }

    if (res == size) {
	return;
    }

    if (res == 0) {
	error("Connection lost!\n");
	return;
    }

    if (res == -1) {
	error("Error on socket: %s\n", strerror(errno));
	return;
    }

    error("Could not write all bytes on socket!\n");
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
Ice_UDPConnection_connect(Ice_ConnectionPtr this, const char* host, int port) {
    struct sockaddr_in addr;
    int res;

    trace();
    Ptr_check(this);

    memset(&addr, 0, sizeof(struct sockaddr_in));

    this->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->fd == -1) {
	error("Could not create socoket: %s\n", strerror(errno));
	return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (! inet_aton(host, &(addr.sin_addr))) {
	error("Invalid host name: %s\n", host);
        return;
    }

    res = connect(this->fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if (res == -1) {
	error("Could not connect: %s\n", strerror(errno));
	return;
    }
}

void
Ice_Connection_listen(Ice_ConnectionPtr this, const char* host, int port) {
    struct sockaddr_in addr;
    int optval;

    trace();
    Ptr_check(this);

    memset(&addr, 0, sizeof(struct sockaddr_in));

    if (this->fd == -1) {
	error("Could not create socoket: %s\n", strerror(errno));
	return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (! inet_aton(host, &(addr.sin_addr))) {
	error("Invalid host name: %s\n", host);
        return;
    }

    optval = 1;
    setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    if (bind(this->fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	error("Could not bind socket: %s\n", strerror(errno));
	return;
    }

    listen(this->fd, 1);
}

void
Ice_UDPConnection_listen(Ice_ConnectionPtr this, const char* host, int port) {
    trace();
    Ptr_check(this);
    this->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    Ice_Connection_listen(this, host, port);
}

void
Ice_TCPConnection_listen(Ice_ConnectionPtr this, const char* host, int port) {
    trace();
    Ptr_check(this);
    this->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Ice_Connection_listen(this, host, port);
}

void
Ice_TCPConnection_accept(Ice_ConnectionPtr this) {
    trace();
    Ptr_check(this);
    this->current_client = accept(this->fd, NULL, 0);
}

void
Ice_ServantMap_init(Ice_ServantMapPtr this) {
    trace();
    this->count = 0;
    Object_init((ObjectPtr)this);
}

void
Ice_EndpointInfo_init(Ice_EndpointInfoPtr this,
		      Ice_EndpointType type,
		      const char* host,
		      int port) {
    trace();

    assert(strlen(host) < MAX_HOST_SIZE);
    strcpy(this->host, host);

    this->type = type;
    this->port = port;
    this->options = NULL;
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
    Ice_EndpointInfoPtr info;
    bool ok;

    trace();
    Ptr_check(this);

    info = &(this->epinfo);

    switch (info->type) {
    case Ice_EndpointTypeUDP:
	Ice_UDPConnection_listen(&(this->connection), info->host, info->port);
	break;

    case Ice_EndpointTypeTCP:
	Ice_TCPConnection_listen(&(this->connection), info->host, info->port);
	break;

    default: {
	ok = IcePlugin_ObjectAdapter_activate(this);

	if (!ok) {
	    throw("Ice_NoEndpointException", __FILE__, __LINE__);
	    return;
	}
    }}

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

    if (this->epinfo.type == Ice_EndpointTypeTCP) {
	Ice_TCPConnection_accept(&(this->connection));
	Ice_Connection_sendValidate(&(this->connection));
	return this->connection.current_client;
    }

    return this->connection.fd;
}

int
Ice_ObjectAdapter_closeClient(Ice_ObjectAdapterPtr this) {
    trace();
    Ptr_check(this);

    if (this->epinfo.type == Ice_EndpointTypeTCP) {
	Ice_Connection_sendClose(&(this->connection));
	close(this->connection.current_client);
	this->connection.current_client = -1;
    }
    return 0;
}

void
Ice_ObjectAdapter_incomingMessage(Ice_ObjectAdapterPtr this) {
    int fd, size, requestId, i;
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

    #ifdef DISSECT_MESSAGE
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
Ice_ObjectPrx_init(Ice_ObjectPrxPtr this,
		   const char* id,
		   Ice_EndpointType type,
		   const char* host,
		   int port) {

    trace();

    Ice_Connection_init(&(this->connection), type);
    Ice_OutputStream_init(&(this->stream));
    Ice_EndpointInfo_init(&(this->epinfo), type, host, port);

    strcpy(this->identity, id);

    /* FIXME: get mode from stringfied proxy */
    this->mode = Ice_ReferenceModeDatagram;

    Object_init((ObjectPtr)this);
}

void
Ice_ObjectPrx_connect(Ice_ObjectPrxPtr this) {
    trace();
    Ptr_check(this);

    if (this->connection.fd == -1) {
	switch (this->epinfo.type) {
	case Ice_EndpointTypeUDP:
	    Ice_UDPConnection_connect(&(this->connection),
				      this->epinfo.host, this->epinfo.port);
	    break;

	default:
	    IcePlugin_ObjectPrx_connect(this);
	}
    }

    assert(this->connection.fd != -1);
}

void
Ice_ObjectPrx_send(Ice_ObjectPrxPtr this) {
    trace();
    Ptr_check(this);

    /* dissect_message(this->stream.data, this->stream.size); */
    Ice_Connection_send(&(this->connection), this->stream.data, this->stream.size);
}

void
Ice_ObjectPrx_readFromInputStream(Ice_ObjectPrxPtr this, Ice_InputStreamPtr is) {
    char identity[MAX_IDENTITY_SIZE];
    char host[MAX_HOST_SIZE];
    byte b, facetSeqSize, numOfEndpoints;
    int port = 0;

    trace();
    Ptr_check(is);

    Ice_InputStream_readIdentity(is, identity);
    Ice_InputStream_readByte(is, &facetSeqSize);
    assert(facetSeqSize == 0);

    Ice_InputStream_readByte(is, &b); /* mode */
    Ice_InputStream_readByte(is, &b); /* secure */
    Ice_InputStream_readByte(is, &numOfEndpoints);
    assert(numOfEndpoints == 1);

    fixme("Not finished implementation");
    host[0] = 0;

    /* FIXME: read endpoint type and use it! */
    Ice_ObjectPrx_init(this, identity, Ice_EndpointTypeUDP, host, port);
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
    Ice_OutputStream_writeShort(os, Ice_EndpointTypeUDP); /* endpoint type */

    Ice_OutputStream_startWriteEncaps(os);
    Ice_OutputStream_writeString(os, this->epinfo.host);  /* host */
    Ice_OutputStream_writeInt(os, this->epinfo.port);     /* port */
    Ice_OutputStream_writeByte(os, protocolMajor);        /* protocolMajor */
    Ice_OutputStream_writeByte(os, protocolMinor);        /* protocolMinor */
    Ice_OutputStream_writeByte(os, encodingMajor);        /* encodingMajor */
    Ice_OutputStream_writeByte(os, encodingMinor);        /* encodingMinor */
    Ice_OutputStream_writeByte(os, 0);                    /* compress */
    Ice_OutputStream_endWriteEncaps(os);
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

    switch(type) {
    case Ice_EndpointTypeTCP:
    case Ice_EndpointTypeUDP: {
	char host[MAX_HOST_SIZE];
	int port;
	IceUtil_getHost(endpoints, host);
	IceUtil_getPort(endpoints, &port);
	Ice_EndpointInfo_init(&(result->epinfo), type, host, port);
	break;
    }

    default:
	IcePlugin_EndpointInfo_init(&(result->epinfo), type, endpoints);
	break;
    }

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
    char identity[MAX_IDENTITY_SIZE];

    trace();
    Ptr_check(this);

    IceUtil_getProtocolType(str, &type);
    IceUtil_getIdentity(str, identity);

    switch(type) {
    case Ice_EndpointTypeTCP:
    case Ice_EndpointTypeUDP: {
	char host[MAX_HOST_SIZE];
	int port;
	IceUtil_getHost(str, host);
	IceUtil_getPort(str, &port);
	Ice_ObjectPrx_init(result, identity, type, host, port);
	break;
    }

    default:
	IcePlugin_ObjectPrx_init(result, identity, type, str);
	break;
    }
}

void
Ice_Communicator_waitForShutdown(Ice_CommunicatorPtr this) {
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

int
IcePlugin_read(Ice_EndpointType type, int fd, byte* data, int size) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;
    int got = 0;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->read(type, fd, data, &size)) {
	    got = size;
    	    break;
	}

    	item = item->next;
    }

    return got;
}

int
IcePlugin_write(Ice_EndpointType type, int fd, const byte* data, int size) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;
    int sent = 0;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

	sent = size;
    	if (endp->write(type, fd, data, &sent))
    	    break;

    	item = item->next;
    }
    return sent;
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
			 const char* id,
			 Ice_EndpointType type,
			 const char* strprx) {
    IcePlugin_PluginListItemPtr item = _registered_endpoints.next;

    trace();

    while(item != NULL) {
    	IcePlugin_EndpointObjectPtr endp = (IcePlugin_EndpointObjectPtr)item->object;
    	Ptr_check(endp);

    	if (endp->ObjectPrx_init(this, id, type, strprx))
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

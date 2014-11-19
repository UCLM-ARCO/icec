/* -*- mode: c; coding: utf-8 -*- */

#include "IceC.h"

int
memncmp(char* p1, char* p2, int size) {
    int i;
    for (i=0; i<size; i++) {
	if (p1[i] > p2[i]) return 1;
	if (p1[i] < p2[i]) return -1;
    }
    return 0;
}

void
Object_init(ObjectPtr this) {
    this->tag = TAG_OK;
}

void
IceUtil_getIdentity(const char* s, char* result) {
    trace();
    s += strspn(s, "\n\r\t ");
    int end = strcspn(s, "\n\r\t ");
    strncpy(result, s, end);
    result[end] = 0;
}

void
IceUtil_getProtocolType(const char* s, Ice_EndpointType* result) {
    trace();

    char proto[MAX_PROTO_SIZE];
    proto[0] = 0;

    s += strspn(s, "\n\r\t ");

    char* i = index(s, ':');
    if (i != NULL) {
	i++;
	s = i + strspn(i, "\n\r\t ");
    }

    int end = strcspn(s, "\n\r\t ");
    strncpy(proto, s, end);
    proto[end] = 0;

    if (streq(proto, "tcp"))
	*result = Ice_EndpointTypeTCP;
    else if (streq(proto, "udp"))
	*result = Ice_EndpointTypeUDP;
    else
	assert(false && "Invalid endpoint type!");
}

void
IceUtil_getHost(const char* s, char* result) {
    trace();
    result[0] = 0;

    while (true) {
	s = index(s, '-');
	if (s == NULL) return;
	if (s[1] == 'h') {
	    s += 2;  // '-h'
	    s += strspn(s, "\n\r\t ");
	    int end = strcspn(s, "\n\r\t ");
	    strncpy(result, s, end);
	    result[end] = 0;
	    return;
	}
	s++;
    }
}

void
IceUtil_getPort(const char* s, int* result) {
    trace();
    *result = -1;
    char strport[10];

    while (true) {
	s = index(s, '-');
	if (s == NULL) return;
	if (s[1] == 'p') {
	    s += 2; // '-p'
	    s += strspn(s, "\n\r\t ");
	    int end = strcspn(s, "\n\r\t ");
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
    }
    }

    this->next = this->data;
    Object_init((ObjectPtr)this);
}

void
Ice_InputStream_readByte(Ice_InputStreamPtr this, Ice_Byte* v) {
    trace();
    Ptr_check(this);

    *v = *(this->next++);
}

void
Ice_InputStream_readShort(Ice_InputStreamPtr this, Ice_Short* v) {
    trace();
    Ptr_check(this);

    if (this->next - this->data + sizeof(Ice_Short) > this->size) {
	throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
	return;
    }

    Ice_Byte* src = this->next;
    this->next += sizeof(Ice_Short);

#ifdef ICE_BIG_ENDIAN
    Ice_Byte* dest = (Ice_Byte*)(v + sizeof(Ice_Short) - 1);
    *dest-- = *src++;
    *dest = *src;
#else
    Ice_Byte* dest = (Ice_Byte*)v;
    *dest++ = *src++;
    *dest = *src;
#endif
}

void
Ice_InputStream_readInt(Ice_InputStreamPtr this, Ice_Int* v) {
    trace();
    Ptr_check(this);

    if (this->next - this->data + sizeof(Ice_Int) > this->size) {
	throw("Ice_UnmarshalOutOfBoundsException", __FILE__, __LINE__);
	return;
    }

    Ice_Byte* src = this->next;
    this->next += sizeof(Ice_Int);

#ifdef ICE_BIG_ENDIAN
    Ice_Byte* dest = (Ice_Byte*)(v + sizeof(Ice_Int) - 1);
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest = *src;
#else
    Ice_Byte* dest = (Ice_Byte*)v;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest = *src;
#endif
}

void
Ice_InputStream_readSize(Ice_InputStreamPtr this, Ice_Int* size, bool increment) {
    trace();
    Ptr_check(this);

    Ice_Byte b;
    Ice_InputStream_readByte(this, &b);
    if (b < 255) {
	*size = b;
	if (! increment) this->next--;
	return;
    }

    Ice_InputStream_readInt(this, size);
    if (! increment) this->next -= sizeof(Ice_Int);
    assert(size >= 0);
}

void
Ice_InputStream_readString(Ice_InputStreamPtr this, char* v) {
    trace();
    Ptr_check(this);

    int size;
    Ice_InputStream_readSize(this, &size, true);
    if (v != NULL)  {
	strncpy(v, this->next, size);
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

    Ice_InputStream_readString(this, name); // name
    Ice_InputStream_readString(this, NULL); // category (not supported)
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
    trace();
    Ptr_check(this);

    byte size = sizeof(Ice_Short);
    assert((this->size + size <= MAX_MESSAGE_SIZE));

    byte* dest = this->next;

#ifdef ICE_BIG_ENDIAN
    byte* src = (byte*)&v + sizeof(Ice_Short) - 1;
    *dest++ = *src--;
    *dest = *src;
#else
    byte* src = (byte*)&v;
    *dest++ = *src++;
    *dest = *src;
#endif

    this->next += size;
    this->size += size;
}

void
Ice_OutputStream_writeInt(Ice_OutputStreamPtr this, Ice_Int v) {
    trace();
    Ptr_check(this);

    byte size = sizeof(Ice_Int);
    assert((this->size + size <= MAX_MESSAGE_SIZE) && "Too long message!");

    byte* dest = this->next;

#ifdef ICE_BIG_ENDIAN
    byte* src = (byte*)&v + sizeof(Ice_Int) - 1;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest = *src;
#else
    byte* src = (byte*)&v;
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
    trace();
    Ptr_check(this);

    int size = strlen(str);
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

    Ice_OutputStream_writeString(this, id); // name
    Ice_OutputStream_writeByte(this, 0);    // category
}

void
Ice_OutputStream_startWriteEncaps(Ice_OutputStreamPtr this) {
    trace();
    Ptr_check(this);

    assert(this->current_encaps + 1 < MAX_NESTED_ENCAPS);
    this->encaps[++(this->current_encaps)] = this->next;

    Ice_OutputStream_writeInt(this, 0); // placeholder for encapsulation length
    Ice_OutputStream_writeByte(this, encodingMajor);
    Ice_OutputStream_writeByte(this, encodingMinor);
}

void
Ice_OutputStream_endWriteEncaps(Ice_OutputStreamPtr this) {
    trace();
    Ptr_check(this);

    Ice_Int* size = (Ice_Int*)this->encaps[this->current_encaps];
    *size = this->next - this->encaps[this->current_encaps];

    this->encaps[(this->current_encaps)--] = NULL;
}

void
Ice_OutputStream_setMessageSize(Ice_OutputStreamPtr this) {
    trace();
    Ptr_check(this);

    byte* next = this->next;
    int size = this->size;

    // Ice header is fixed length, size starts at position 11
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
    trace();
    Ptr_check(this);

    int fd = this->current_client;
    if (fd == -1) fd = this->fd;

    assert(this->fd > -1);

    int res;
    switch (this->type) {
    case Ice_EndpointTypeTCP:
    case Ice_EndpointTypeUDP: {
	res = send(fd, data, size, 0);
	break;
    }
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
    trace();
    Ptr_check(this);

    // FIXME: there is no need for OutputStream here
    Ice_OutputStream os;
    Ice_OutputStream_init(&os);
    Ice_OutputStream_writeBlob(&os, Ice_header, 8);
    Ice_OutputStream_writeByte(&os, Ice_validateConnectionMsg);
    Ice_OutputStream_writeByte(&os, 0);  // compression
    Ice_OutputStream_writeInt(&os, 14);

    Ice_Connection_send(this, os.data, os.size);
}

void
Ice_Connection_sendClose(Ice_ConnectionPtr this) {
    trace();
    Ptr_check(this);

    // FIXME: there is no need for OutputStream here
    Ice_OutputStream os;
    Ice_OutputStream_init(&os);
    Ice_OutputStream_writeBlob(&os, Ice_header, 8);
    Ice_OutputStream_writeByte(&os, Ice_closeConnectionMsg);
    Ice_OutputStream_writeByte(&os, 0);  // compression
    Ice_OutputStream_writeInt(&os, 14);

    Ice_Connection_send(this, os.data, os.size);
}

void
Ice_UDPConnection_connect(Ice_ConnectionPtr this, const char* host, int port) {
    trace();
    Ptr_check(this);

    struct sockaddr_in addr;
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

    int res = connect(this->fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if (res == -1) {
	error("Could not connect: %s\n", strerror(errno));
	return;
    }
}

void
Ice_Connection_listen(Ice_ConnectionPtr this, const char* host, int port) {
    trace();
    Ptr_check(this);

    struct sockaddr_in addr;
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

    int optval = 1;
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

    this->type = type;
    Object_init((ObjectPtr)this);

    assert(strlen(host) < MAX_HOST_SIZE);
    strcpy(this->host, host);
    this->port = port;
}

void
Ice_ObjectAdapter_init(Ice_ObjectAdapterPtr this,
		       Ice_CommunicatorPtr communicator,
		       Ice_EndpointType proto,
		       const char* host,
		       int port) {

    trace();
    Ptr_check(communicator);

    this->communicator = communicator;
    this->activated = false;

    Ice_EndpointInfo_init(&(this->epinfo), proto, host, port);
    Ice_Connection_init(&(this->connection), proto);
    Ice_ServantMap_init(&(this->servantMap));
    Object_init((ObjectPtr)this);
}


void
Ice_ObjectAdapter_activate(Ice_ObjectAdapterPtr this) {
    trace();
    Ptr_check(this);

    Ice_EndpointInfoPtr info = &(this->epinfo);

    switch (info->type) {
    case Ice_EndpointTypeUDP:
	Ice_UDPConnection_listen(&(this->connection), info->host, info->port);
	break;

    case Ice_EndpointTypeTCP:
	Ice_TCPConnection_listen(&(this->connection), info->host, info->port);
	break;

    default:
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
    trace();
    Ptr_check(this);
    Ptr_check(servant);
    assert(strlen(identity) < MAX_IDENTITY_SIZE);

    Ice_ServantMapPtr sm = &(this->servantMap);
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
}

void
Ice_ObjectAdapter_incomingMessage(Ice_ObjectAdapterPtr this) {
    trace();
    Ptr_check(this);

    int fd = Ice_ObjectAdapter_getClient(this);

    Ice_InputStream is;
    Ice_InputStream_init(&is, fd, this->connection.type);

    char header[] = {
	'I', 'c', 'e', 'P',    // int  magic
	1,                     // byte protocolMajor
	0,                     // byte protocolMinor
	1,                     // byte encodingMajor
	0,                     // byte encodingMinor
	0,                     // byte messageType (0 -> request)
	0};                    // byte compressionStatus (0 -> no compress)

    if (memncmp(header, is.next, 10) != 0) {
	throw("Ice_ProtocolException", __FILE__, __LINE__);
	Ice_ObjectAdapter_closeClient(this);
	return;
    }
    is.next += 10;

    int size;
    Ice_InputStream_readInt(&is, &size);
    if (size > MAX_MESSAGE_SIZE) {
	throw("Ice_MemoryLimitException", __FILE__, __LINE__);
	Ice_ObjectAdapter_closeClient(this);
	return;
    }

    // FIXME: this should not be ignored
    int requestId;
    Ice_InputStream_readInt(&is, &requestId);

    char identity[MAX_IDENTITY_SIZE];
    Ice_InputStream_readIdentity(&is, identity);

    // facet not supported
    byte facetSeqSize;
    Ice_InputStream_readByte(&is, &facetSeqSize);
    assert(facetSeqSize == 0);

    int i;
    for (i=0; i<this->servantMap.count; i++) {
	if (streq(this->servantMap.ids[i], identity)) {

	    Ice_ObjectPtr servant = this->servantMap.objects[i];
	    assert(servant->handler != NULL);

	    // prepare reply message
	    Ice_OutputStream os;
	    Ice_OutputStream_init(&os);

	    // reply header
	    Ice_OutputStream_writeBlob(&os, Ice_header, 8);
	    Ice_OutputStream_writeByte(&os, Ice_replyMsg);
	    Ice_OutputStream_writeByte(&os, 0);  // compression
	    Ice_OutputStream_writeInt(&os, 0);  // size (placeholder)

	    // reply data
	    Ice_OutputStream_writeInt(&os, requestId);
	    Ice_OutputStream_writeByte(&os, 0);  // reply status

	    // user data encapsulation
	    Ice_OutputStream_startWriteEncaps(&os);
	    servant->handler(servant, &is, &os);
	    Ice_OutputStream_endWriteEncaps(&os);
	    Ice_OutputStream_setMessageSize(&os);
	    Ice_Connection_send(&(this->connection), os.data, os.size);

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

    // FIXME: get mode from stringfied proxy
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
	}
    }

    assert(this->connection.fd != -1);
}

void
Ice_ObjectPrx_send(Ice_ObjectPrxPtr this) {
    trace();
    Ptr_check(this);

    // dissect_message(this->stream.data, this->stream.size);
    Ice_Connection_send(&(this->connection), this->stream.data, this->stream.size);
}

void
Ice_ObjectPrx_readFromInputStream(Ice_ObjectPrxPtr this, Ice_InputStreamPtr is) {
    trace();
    Ptr_check(is);

    char identity[MAX_IDENTITY_SIZE];
    Ice_InputStream_readIdentity(is, identity);

    byte facetSeqSize;
    Ice_InputStream_readByte(is, &facetSeqSize);
    assert(facetSeqSize == 0);

    byte b;
    Ice_InputStream_readByte(is, &b); // mode
    Ice_InputStream_readByte(is, &b); // secure

    byte numOfEndpoints;
    Ice_InputStream_readByte(is, &b);
    assert(numOfEndpoints == 1);

    fixme("Not finished implementation");
    char host[MAX_HOST_SIZE];
    host[0] = 0;
    int port = 0;

    // FIXME: read endpoint type and use it!
    Ice_ObjectPrx_init(this, identity, Ice_EndpointTypeUDP, host, port);
}

void
Ice_ObjectPrx_writeToOutputStream(Ice_ObjectPrxPtr this, Ice_OutputStreamPtr os) {
    trace();
    Ptr_check(os);

    // if null proxy, write empty Identity (0 0) and return
    if (! this) {
	Ice_OutputStream_writeByte(os, 0);
	Ice_OutputStream_writeByte(os, 0);
	return;
    }

    Ptr_check(this);

    Ice_OutputStream_writeIdentity(os, this->identity);   // identity
    Ice_OutputStream_writeByte(os, 0);                    // facet
    Ice_OutputStream_writeByte(os, this->mode);           // mode
    Ice_OutputStream_writeByte(os, 0);                    // secure

    Ice_OutputStream_writeByte(os, 1);                    // num of endpoints
    Ice_OutputStream_writeShort(os, Ice_EndpointTypeUDP); // endpoint type

    Ice_OutputStream_startWriteEncaps(os);
    Ice_OutputStream_writeString(os, this->epinfo.host);  // host
    Ice_OutputStream_writeInt(os, this->epinfo.port);     // port
    Ice_OutputStream_writeByte(os, protocolMajor);        // protocolMajor
    Ice_OutputStream_writeByte(os, protocolMinor);        // protocolMinor
    Ice_OutputStream_writeByte(os, encodingMajor);        // encodingMajor
    Ice_OutputStream_writeByte(os, encodingMinor);        // encodingMinor
    Ice_OutputStream_writeByte(os, 0);                    // compress
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
    trace();
    Ptr_check(this);

    Ice_EndpointType type;
    char host[MAX_HOST_SIZE];
    int port;

    IceUtil_getProtocolType(endpoints, &type);
    IceUtil_getHost(endpoints, host);
    IceUtil_getPort(endpoints, &port);

    Ice_ObjectAdapter_init(result, this, type, host, port);
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
    trace();
    Ptr_check(this);

    Ice_EndpointType type;
    char identity[MAX_IDENTITY_SIZE];
    char host[MAX_HOST_SIZE];
    int port;

    IceUtil_getProtocolType(str, &type);
    IceUtil_getIdentity(str, identity);
    IceUtil_getHost(str, host);
    IceUtil_getPort(str, &port);

    Ice_ObjectPrx_init(result, identity, type, host, port);
}

void
Ice_Communicator_waitForShutdown(Ice_CommunicatorPtr this) {
    trace();
    Ptr_check(this);

    int i, count;
    fd_set read_set;
    Ice_ObjectAdapterPtr adapter;

    int fds[MAX_NUM_OF_ADAPTERS];
    Ice_ObjectAdapterPtr adapters[MAX_NUM_OF_ADAPTERS];

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
	    error("Error while reading from sockets: %s\n", strerror(errno));
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

void
Ice_initialize(Ice_CommunicatorPtr ic) {
    trace();
    Ice_Communicator_init(ic);
}

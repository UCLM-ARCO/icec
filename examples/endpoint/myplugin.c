/* -*- mode: c; coding: utf-8 -*- */

#include "myplugin.h"

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static MyPlugin_EndpointOptions ep_options;

bool
MyPlugin_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (!streq(proto, "file"))
    	return false;

    *result = Ice_EndpointTypeFile;
    return true;
}

bool
MyPlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr this) {
    MyPlugin_EndpointOptionsPtr options =
	(MyPlugin_EndpointOptionsPtr)this->epinfo.options;

    trace();
    if (this->epinfo.type != Ice_EndpointTypeFile)
	return false;

    MyPlugin_Connection_open_file(&(this->connection), options->path, true);
    return true;
}

bool
MyPlugin_EndpointInfo_init(Ice_EndpointInfoPtr this, Ice_EndpointType type,
			   const char* endpoint) {
    trace();
    if (type != Ice_EndpointTypeFile)
    	return false;

    this->type = type;
    this->options = &ep_options;
    Object_init((ObjectPtr)this);

    MyPlugin_Endpoint_getPath(endpoint, ep_options.path);
    return true;
}

bool
MyPlugin_ObjectPrx_connect(Ice_ObjectPrxPtr this) {
    MyPlugin_EndpointOptionsPtr options =
	(MyPlugin_EndpointOptionsPtr)this->epinfo.options;

    trace();
    if (this->epinfo.type != Ice_EndpointTypeFile)
	return false;

    MyPlugin_Connection_open_file(&(this->connection), options->path, false);
    return true;
}

bool
MyPlugin_ObjectPrx_init(Ice_ObjectPrxPtr this, const char* id, Ice_EndpointType type,
			const char* strprx) {
    trace();
    if (type != Ice_EndpointTypeFile)
	return false;

    Ice_Connection_init(&(this->connection), type);
    Ice_OutputStream_init(&(this->stream));

    MyPlugin_EndpointInfo_init(&(this->epinfo), type, strprx);
    strcpy(this->identity, id);

    /* FIXME: get mode from stringfied proxy */
    this->mode = Ice_ReferenceModeOneway;

    Object_init((ObjectPtr)this);
    return true;
}

bool
MyPlugin_write(Ice_EndpointType type, int fd, const byte* data, int* size) {
    trace();
    if (type != Ice_EndpointTypeFile)
	return false;

    *size = write(fd, data, *size);
    return true;
}

bool
MyPlugin_read(Ice_EndpointType type, int fd, byte* data, int* size) {
    trace();
    if (type != Ice_EndpointTypeFile)
	return false;

    *size = read(fd, data, *size);
    return true;
}

void
MyPlugin_init(Ice_CommunicatorPtr ic) {
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

    endpoint.getProtocolType = &MyPlugin_getProtocolType;
    endpoint.ObjectAdapter_activate = &MyPlugin_ObjectAdapter_activate;
    endpoint.EndpointInfo_init = &MyPlugin_EndpointInfo_init;
    endpoint.ObjectPrx_init = &MyPlugin_ObjectPrx_init;
    endpoint.ObjectPrx_connect = &MyPlugin_ObjectPrx_connect;
    endpoint.read = &MyPlugin_read;
    endpoint.write = &MyPlugin_write;
}

void
MyPlugin_Endpoint_getPath(const char* s, char* result) {
    int end;

    trace();

    result[0] = 0;
    while (true) {
    	s = index(s, '-');
    	if (s == NULL) return;
    	if (s[1] == 'p') {
    	    s += 2;  /* '-p' */
    	    s += strspn(s, "\n\r\t ");
    	    end = strcspn(s, "\n\r\t ");
	    assert(end <= MAX_PATH_SIZE && "Path size too big!");
    	    strncpy(result, s, end);
    	    result[end] = 0;
    	    return;
    	}
    	s++;
    }
}

void
MyPlugin_Connection_open_file(Ice_ConnectionPtr this, const char* path, bool oaendpoint) {
    trace();

    if (oaendpoint) {
	unlink(path);
	if (mkfifo(path, 0600) < 0) {
	    error("Error creating FIFO: %s\n", strerror(errno));
	    throw("MyPlugin_IOError", __FILE__, __LINE__);
	    return;
	}
    }

    this->fd = open(path, O_RDWR | O_SYNC, S_IRUSR | S_IWUSR);
    if (this->fd == -1) {
	error("Error openning file: %s\n", strerror(errno));
	throw("MyPlugin_IOError", __FILE__, __LINE__);
	return;
    }
}

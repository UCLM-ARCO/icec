/* -*- mode: c; coding: utf-8 -*- */

#include "myplugin.h"

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;

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
}

bool
MyPlugin_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (!streq(proto, "file"))
    	return false;

    printf(" file -> %d\n",  Ice_EndpointTypeFile);
    *result = Ice_EndpointTypeFile;
    return true;
}

bool
MyPlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr this) {
    trace();

    printf("this->epinfo.type: %d\n", this->epinfo.type);
    if (this->epinfo.type != Ice_EndpointTypeFile)
	return false;

    MyPlugin_EndpointInfoPtr info = (MyPlugin_EndpointInfoPtr)&(this->epinfo);
    printf("path: '%s'\n", info->path);
    return true;
}

bool
MyPlugin_EndpointInfo_init(Ice_EndpointInfoPtr this_, Ice_EndpointType type,
			   const char* endpoint) {
    trace();
    if (type != Ice_EndpointTypeFile)
    	return false;

    this_->type = type;
    Object_init((ObjectPtr)this_);

    MyPlugin_EndpointInfoPtr this = (MyPlugin_EndpointInfoPtr)this_;
    // MyPlugin_Endpoint_getPath(endpoint, this->path);
    return true;
}

void
MyPlugin_Endpoint_getPath(const char* s, char* result) {
    trace();

    result[0] = 0;
    while (true) {
    	s = index(s, '-');
    	if (s == NULL) return;
    	if (s[1] == 'p') {
    	    s += 2;  // '-p'
    	    s += strspn(s, "\n\r\t ");
    	    int end = strcspn(s, "\n\r\t ");
    	    strncpy(result, s, end);
    	    result[end] = 0;
    	    return;
    	}
    	s++;
    }
}

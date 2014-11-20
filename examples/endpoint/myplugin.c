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
}

bool
MyPlugin_getProtocolType(const char* proto, Ice_EndpointType* result) {
    if (streq(proto, "file"))
	*result = Ice_EndpointTypeFile;
}

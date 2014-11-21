/* -*- mode: c; coding: utf-8 -*- */

#ifndef _MYPLUGIN_H_
#define _MYPLUGIN_H_

#include "IceC/IceC.h"

#define Ice_EndpointTypeFile 11

/*
   NOTE: this could not be done; the Adapter->epinfo field (which is
   the actual instance) is of type Ice_EndpointInfo, not this type.
   Maybe the solution is: adapter has a pointer to Ice_EndpointInfo,
   and in other place there is the actual instance (a static var
   somewhere, ie. in plugins space). This lead to a single-adapter
   condition...

   Better than this: add a void pointer on Ice_EndpointInfo, and each
   plugin should point this to its internal structure.
 */


/* typedef struct { */
/*     Ice_EndpointInfo _base; */

/*     char path[64]; */
/* } MyPlugin_EndpointInfo; */

typedef MyPlugin_EndpointInfo* MyPlugin_EndpointInfoPtr;


void MyPlugin_init(Ice_CommunicatorPtr ic);
bool MyPlugin_getProtocolType(const char* proto, Ice_EndpointType* result);
bool MyPlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr this);
bool MyPlugin_EndpointInfo_init(Ice_EndpointInfoPtr this, Ice_EndpointType type,
				const char* endpoint);
void MyPlugin_Endpoint_getPath(const char* s, char* result);



#endif // _MYPLUGIN_H_

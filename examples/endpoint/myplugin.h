/* -*- mode: c; coding: utf-8 -*- */

#ifndef _MYPLUGIN_H_
#define _MYPLUGIN_H_

#include "IceC/IceC.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define Ice_EndpointTypeFile 11
#define MAX_PATH_SIZE        64

typedef struct MyPlugin_EndpointOptions {
    char path[MAX_PATH_SIZE];
} MyPlugin_EndpointOptions;

typedef MyPlugin_EndpointOptions* MyPlugin_EndpointOptionsPtr;

bool MyPlugin_getProtocolType(const char* proto, Ice_EndpointType* result);
bool MyPlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr this);
bool MyPlugin_EndpointInfo_init(Ice_EndpointInfoPtr this, Ice_EndpointType type,
				const char* endpoint);
bool MyPlugin_ObjectPrx_connect(Ice_ObjectPrxPtr this);
bool MyPlugin_ObjectPrx_init(Ice_ObjectPrxPtr this, const char* id, Ice_EndpointType type,
			     const char* strprx);
bool MyPlugin_Connection_send(Ice_ConnectionPtr this, int fd, byte* data,
			      int size, int* sent);
bool MyPlugin_InputStream_init(Ice_InputStreamPtr this, int fd, Ice_EndpointType type);

void MyPlugin_init(Ice_CommunicatorPtr ic);
void MyPlugin_Endpoint_getPath(const char* s, char* result);
void MyPlugin_Connection_open_file(Ice_ConnectionPtr this, const char* path, bool oaendp);


#endif /* _MYPLUGIN_H_ */

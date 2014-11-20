/* -*- mode: c; coding: utf-8 -*- */

#ifndef _MYPLUGIN_H_
#define _MYPLUGIN_H_

#include "IceC/IceC.h"

#define Ice_EndpointTypeFile 11

void MyPlugin_init(Ice_CommunicatorPtr ic);
bool MyPlugin_getProtocolType(const char* proto, Ice_EndpointType* result);

#endif // _MYPLUGIN_H_

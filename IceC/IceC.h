/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ICEC_H_
#define _ICE_ICEC_H_

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>
#include <errno.h>
#include <sys/select.h>

#include "types.h"
#include "debug.h"

#if defined(__i386)  || defined(_M_IX86)    || defined(__x86_64)   ||	\
    defined(_M_X64)  || defined(_M_IA64)    || defined(__alpha__)  ||	\
    defined(_M_ARM)  || defined(__MIPSEL__) || defined(__ARMEL__)  ||	\
    defined(__BFIN__)
#   define ICE_LITTLE_ENDIAN
#elif defined(__sparc) || defined(__sparc__) || defined(__hppa)    ||	\
    defined(__ppc__)   || defined(__powerpc) || defined(_ARCH_COM) ||	\
    defined(__MIPSEB__)
#   define ICE_BIG_ENDIAN
#else
#    error "Unknown architecture"
#endif

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE        128
#endif

#define MAX_IDENTITY_SIZE       16
#define MAX_NESTED_ENCAPS       5
#define MAX_NUM_OF_ADAPTERS     5
#define MAX_NUM_OF_SERVANTS     5
#define MAX_PROTO_SIZE          16
#define MAX_OPERATION_NAME_SIZE 16

#define streq(S1, S2) (strcmp(S1, S2) == 0)

#define TAG_OK   4444
#define TAG_NULL 0

#define Ptr_check(ptr) assert(ptr); assert(((ObjectPtr)ptr)->tag == TAG_OK);
#define throw(EXC, FILE, LINE) error(EXC " %s:%d\n", FILE, LINE); assert(false);

typedef bool Ice_Bool;
typedef uint8_t Ice_Byte;
typedef int16_t Ice_Short;
typedef int32_t Ice_Int;
typedef int64_t Ice_Long;

#define magic0 0x49 /* I */
#define magic1 0x63 /* c */
#define magic2 0x65 /* e */
#define magic3 0x50 /* P */

#define protocolMajor 1
#define protocolMinor 0
#define encodingMajor 1
#define encodingMinor 0

typedef enum {
    Ice_requestMsg,
    Ice_requestBatchMsg,
    Ice_replyMsg,
    Ice_validateConnectionMsg,
    Ice_closeConnectionMsg
} Ice_MessageType;

typedef enum {
    Ice_Normal,
    Ice_Nonmutating,
    Ice_Idempotent
} Ice_OperationMode;

typedef enum {
    Ice_ReferenceModeTwoway,
    Ice_ReferenceModeOneway,
    Ice_ReferenceModeBatchOneway,
    Ice_ReferenceModeDatagram,
    Ice_ReferenceModeBatchDatagram
} Ice_ReferenceMode;

#define Ice_EndpointTypeTCP 1
#define Ice_EndpointTypeSSL 2
#define Ice_EndpointTypeUDP 3

typedef byte Ice_EndpointType;

static const byte Ice_header[] = {
    magic0,
    magic1,
    magic2,
    magic3,
    protocolMajor,
    protocolMinor,
    encodingMajor,
    encodingMinor,
    Ice_requestMsg,
    0,               /* Compression status */
    0, 0, 0, 0,      /* Message size (placeholder) */
};

typedef struct {
    int tag;
} Object;

typedef Object* ObjectPtr;

void Object_init(ObjectPtr this);

void IceUtil_getIdentity(const char* s, char* result);
void IceUtil_getProtocolType(const char* s, Ice_EndpointType* result);
void IceUtil_getMode(const char* s, Ice_ReferenceMode* mode);

/* Forward declaration */
struct Ice_InputStream;
typedef struct Ice_InputStream* Ice_InputStreamPtr;

struct Ice_OutputStream;
typedef struct Ice_OutputStream* Ice_OutputStreamPtr;

struct Ice_Object;
typedef struct Ice_Object* Ice_ObjectPtr;

typedef void (*Ice_MethodHandlerPtr)(Ice_ObjectPtr,
				     Ice_InputStreamPtr,
				     Ice_OutputStreamPtr);

typedef struct Ice_Object {
    Object _base;
    Ice_MethodHandlerPtr handler;
} Ice_Object;

void Ice_Object_init(Ice_ObjectPtr this, Ice_MethodHandlerPtr handler);

typedef struct Ice_InputStream {
    Object _base;

    byte data[MAX_MESSAGE_SIZE];
    byte* next;
    int size;
} Ice_InputStream;

void Ice_InputStream_init(Ice_InputStreamPtr this, int fd, Ice_EndpointType type);
void Ice_InputStream_readBool(Ice_InputStreamPtr this, Ice_Bool* v);
void Ice_InputStream_readByte(Ice_InputStreamPtr this, Ice_Byte* v);
void Ice_InputStream_readShort(Ice_InputStreamPtr this, Ice_Short* v);
void Ice_InputStream_readInt(Ice_InputStreamPtr this, Ice_Int* v);
void Ice_InputStream_readSize(Ice_InputStreamPtr this, Ice_Int* size, bool increment);
void Ice_InputStream_readString(Ice_InputStreamPtr this, char* v);
void Ice_InputStream_readBlob(Ice_InputStreamPtr this, byte* v, int size);
void Ice_InputStream_readIdentity(Ice_InputStreamPtr this, char* name);

typedef struct Ice_OutputStream {
    Object _base;

    byte data[MAX_MESSAGE_SIZE];
    byte* next;
    int size;
    byte* encaps[MAX_NESTED_ENCAPS];
    int current_encaps;
} Ice_OutputStream;

void Ice_OutputStream_init(Ice_OutputStreamPtr this);
void Ice_OutputStream_writeBool(Ice_OutputStreamPtr this, Ice_Bool v);
void Ice_OutputStream_writeByte(Ice_OutputStreamPtr this, Ice_Byte v);
void Ice_OutputStream_writeShort(Ice_OutputStreamPtr this, Ice_Short v);
void Ice_OutputStream_writeInt(Ice_OutputStreamPtr this, Ice_Int v);
void Ice_OutputStream_writeBlob(Ice_OutputStreamPtr this, const byte* data, int size);
void Ice_OutputStream_writeSize(Ice_OutputStreamPtr this, int size);
void Ice_OutputStream_writeString(Ice_OutputStreamPtr this, const char* str);
void Ice_OutputStream_writeIdentity(Ice_OutputStreamPtr this, const char* id);
void Ice_OutputStream_startWriteEncaps(Ice_OutputStreamPtr this);
void Ice_OutputStream_endWriteEncaps(Ice_OutputStreamPtr this);
void Ice_OutputStream_setMessageSize(Ice_OutputStreamPtr this);

typedef struct {
    Object _base;

    Ice_EndpointType type;
    int fd;
    int current_client;
} Ice_Connection;

typedef Ice_Connection* Ice_ConnectionPtr;

void Ice_Connection_init(Ice_ConnectionPtr this, Ice_EndpointType type);
void Ice_Connection_send(Ice_ConnectionPtr this, byte* data, int size);
void Ice_Connection_sendValidate(Ice_ConnectionPtr this);
void Ice_Connection_sendClose(Ice_ConnectionPtr this);

/* Forward declaration */
struct Ice_Communicator;
typedef struct Ice_Communicator* Ice_CommunicatorPtr;

typedef struct {
    Object _base;

    char ids[MAX_NUM_OF_SERVANTS][MAX_IDENTITY_SIZE];
    Ice_ObjectPtr objects[MAX_NUM_OF_SERVANTS];
    int count;
} Ice_ServantMap;

typedef Ice_ServantMap* Ice_ServantMapPtr;

void Ice_ServantMap_init(Ice_ServantMapPtr this);

typedef struct {
    Object _base;

    Ice_EndpointType type;
    bool datagram;
    void* options;
} Ice_EndpointInfo;

typedef Ice_EndpointInfo* Ice_EndpointInfoPtr;

typedef struct {
    Object _base;

    Ice_CommunicatorPtr communicator;
    Ice_Connection connection;
    bool activated;
    Ice_ServantMap servantMap;
    Ice_EndpointInfo epinfo;
} Ice_ObjectAdapter;

typedef Ice_ObjectAdapter* Ice_ObjectAdapterPtr;

void Ice_ObjectAdapter_init(Ice_ObjectAdapterPtr this,
			    Ice_CommunicatorPtr communicator);
void Ice_ObjectAdapter_activate(Ice_ObjectAdapterPtr this);
void Ice_ObjectAdapter_add(Ice_ObjectAdapterPtr this,
			   Ice_ObjectPtr servant,
			   const char* identity);
int Ice_ObjectAdapter_getClient(Ice_ObjectAdapterPtr this);
int Ice_ObjectAdapter_closeClient(Ice_ObjectAdapterPtr this);
void Ice_ObjectAdapter_incomingMessage(Ice_ObjectAdapterPtr this);

typedef struct {
    Object _base;
    Ice_ObjectAdapterPtr begin[MAX_NUM_OF_ADAPTERS];
    int count;
} Ice_ObjectAdapter_List;

typedef Ice_ObjectAdapter_List* Ice_ObjectAdapter_ListPtr;

void Ice_ObjectAdapter_List_init(Ice_ObjectAdapter_ListPtr this);

typedef struct {
    Object _base;

    Ice_Connection connection;
    Ice_OutputStream stream;
    char identity[MAX_IDENTITY_SIZE];
    Ice_ReferenceMode mode;
    Ice_EndpointInfo epinfo;
} Ice_ObjectPrx;

typedef Ice_ObjectPrx* Ice_ObjectPrxPtr;

void Ice_ObjectPrx_connect(Ice_ObjectPrxPtr this);
void Ice_ObjectPrx_closeConnection(Ice_ObjectPrxPtr this);
void Ice_ObjectPrx_send(Ice_ObjectPrxPtr this);
void Ice_ObjectPrx_readFromInputStream(Ice_ObjectPrxPtr this, Ice_InputStreamPtr is);
void Ice_ObjectPrx_writeToOutputStream(Ice_ObjectPrxPtr this, Ice_OutputStreamPtr os);

typedef struct Ice_Communicator {
    Object _base;
    Ice_ObjectAdapter_List adapters;
} Ice_Communicator;

void Ice_Communicator_init(Ice_CommunicatorPtr this);
void Ice_Communicator_createObjectAdapterWithEndpoints(Ice_CommunicatorPtr this,
						       const char* name,
						       const char* endpoints,
						       Ice_ObjectAdapterPtr result);
void Ice_Communicator_registerAdapter(Ice_CommunicatorPtr this,
				      Ice_ObjectAdapterPtr adapter);
void Ice_Communicator_stringToProxy(Ice_CommunicatorPtr this, const char* str,
				    Ice_ObjectPrx* result);
void Ice_Communicator_waitForShutdown(Ice_CommunicatorPtr this);
void Ice_initialize(Ice_CommunicatorPtr ic);

typedef struct IcePlugin_PluginListItem* IcePlugin_PluginListItemPtr;

typedef struct IcePlugin_PluginListItem {
    Object _base;

    ObjectPtr object;
    IcePlugin_PluginListItemPtr next;
} IcePlugin_PluginListItem;

void IcePlugin_PluginListItem_init(IcePlugin_PluginListItemPtr this, ObjectPtr object);

typedef struct IcePlugin_EndpointObject {
    Object _base;

    bool (*getProtocolType)(const char*, Ice_EndpointType*);
    bool (*InputStream_init)(Ice_InputStreamPtr, int, Ice_EndpointType);
    bool (*ObjectAdapter_activate)(Ice_ObjectAdapterPtr);
    bool (*EndpointInfo_init)(Ice_EndpointInfoPtr, Ice_EndpointType, const char*);

    bool (*ObjectPrx_init)(Ice_ObjectPrxPtr, Ice_EndpointType, const char*);
    bool (*ObjectPrx_connect)(Ice_ObjectPrxPtr);

    bool (*Connection_accept)(Ice_ConnectionPtr);
    bool (*Connection_send)(Ice_ConnectionPtr, int, byte*, uint16_t*);
    bool (*Connection_close)(Ice_ConnectionPtr);

} IcePlugin_EndpointObject;

typedef IcePlugin_EndpointObject* IcePlugin_EndpointObjectPtr;

void IcePlugin_registerEndpoint(IcePlugin_PluginListItemPtr item);
void IcePlugin_EndpointObject_init(IcePlugin_EndpointObjectPtr this);

void IcePlugin_getProtocolType(const char* proto, Ice_EndpointType* result);
void IcePlugin_InputStream_init(Ice_InputStreamPtr this, int fd, Ice_EndpointType type);
void IcePlugin_Connection_accept(Ice_ConnectionPtr this);
int IcePlugin_Connection_send(Ice_ConnectionPtr this, int fd, byte* data, uint16_t size);
void IcePlugin_Connection_close(Ice_ConnectionPtr this);
void IcePlugin_EndpointInfo_init(Ice_EndpointInfoPtr this,
				 Ice_EndpointType type,
				 const char* endpoint);
bool IcePlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr this);
void IcePlugin_ObjectPrx_init(Ice_ObjectPrxPtr this,
			      Ice_EndpointType type,
			      const char* strprx);
void IcePlugin_ObjectPrx_connect(Ice_ObjectPrxPtr this);

#endif  /* _ICE_ICEC_H_ */

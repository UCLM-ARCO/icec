/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#ifndef _ICE_ICEC_H_
#define _ICE_ICEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <IceC/platform.h>

#define streq(S1, S2) (strcmp(S1, S2) == 0)

#define TAG_OK 4444

#define Ptr_check(ptr)                                                  \
    do {                                                                \
        assert(ptr && "Invalid pointer");                               \
        assert(((ObjectPtr)ptr)->tag == TAG_OK && "Uninitialized");     \
    } while(0)

#define c_throw(EXC, FILE, LINE)                                        \
    do {                                                                \
        error_at_line(0, 0, FILE, LINE, EXC);                           \
        assert(false && "Exception c_thrown");                          \
    } while(0)

#ifdef __STDC_IEC_559__
  #define _HAS_32BIT_FLOAT_ 1
#else
  #pragma message("Your compiler does not support IEEE 754 (single precision) float")
#endif

typedef bool Ice_Bool;
typedef uint8_t Ice_Byte;
typedef int16_t Ice_Short;
typedef int32_t Ice_Int;
typedef int64_t Ice_Long;

#ifdef _HAS_32BIT_FLOAT_
  typedef float Ice_Float;
#endif

typedef struct Ice_String {
    Ice_Int size;
    const char* value;
} Ice_String;

typedef Ice_String* Ice_StringPtr;

#define Ice_String_init(NAME, VALUE)            \
    do {                                        \
        NAME.value = VALUE;                     \
        NAME.size = strlen(VALUE);              \
    } while(0);

#define Ice_String_eq(A, B) (strncmp(A.value, B, A.size) == 0)

#define Ice_Dict_init(DICT, DICT_ITEMS, SIZE)   \
    do {                                        \
        DICT.size = SIZE;                       \
        DICT.items = DICT_ITEMS;                \
    } while(0);

#define Ice_Dict_set(DICT, INDEX, KEY, VALUE)   \
    do {                                        \
        DICT.items[INDEX].key = KEY;            \
        DICT.items[INDEX].value = VALUE;        \
    } while(0);

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

typedef byte Ice_EndpointType;

static const byte Ice_header[] = {
    0x49,            /* I */
    0x63,            /* c */
    0x65,            /* e */
    0x50,            /* P */
    protocolMajor,
    protocolMinor,
    encodingMajor,
    encodingMinor,
    Ice_requestMsg,
    0,               /* Compression status */
    0, 0, 0, 0,      /* Message size (placeholder) */
};

typedef struct Object {
    int tag;
} Object;

typedef Object* ObjectPtr;

void Object_init(ObjectPtr self);

/* Forward declaration */
struct Ice_InputStream;
typedef struct Ice_InputStream* Ice_InputStreamPtr;

struct Ice_OutputStream;
typedef struct Ice_OutputStream* Ice_OutputStreamPtr;

struct Ice_Object;
typedef struct Ice_Object* Ice_ObjectPtr;

typedef struct Ice_Current {
    char* id;
    char* operation;
} Ice_Current;

typedef void (*Ice_MethodHandlerPtr)(Ice_ObjectPtr,
                                     Ice_InputStreamPtr,
                                     Ice_OutputStreamPtr);

typedef struct Ice_Object {
    Object _base;
    Ice_MethodHandlerPtr handler;
} Ice_Object;

typedef struct Ice_Blobject* Ice_BlobjectPtr;

void Ice_Object_init(Ice_ObjectPtr self,
                     Ice_MethodHandlerPtr handler);

typedef void (*Ice_Blobject_HandlerPtr)(Ice_BlobjectPtr,
                                        Ice_InputStreamPtr,
                                        Ice_Current,
                                        Ice_OutputStreamPtr);

typedef struct Ice_Blobject {
    Ice_Object _base;
    Ice_Blobject_HandlerPtr user_handler;
} Ice_Blobject;

void Ice_Blobject_dispatch(Ice_ObjectPtr self,
                           Ice_InputStreamPtr is,
                           Ice_OutputStreamPtr os);
void Ice_Blobject_init(Ice_BlobjectPtr self,
                       Ice_Blobject_HandlerPtr handler);

typedef struct Ice_InputStream {
    Object _base;

    byte data[MAX_MESSAGE_SIZE];
    byte* next;
    int size;
} Ice_InputStream;

void Ice_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type);
void Ice_InputStream_readBool(Ice_InputStreamPtr self, Ice_Bool* v);
void Ice_InputStream_readByte(Ice_InputStreamPtr self, Ice_Byte* v);
void Ice_InputStream_readShort(Ice_InputStreamPtr self, Ice_Short* v);
void Ice_InputStream_readInt(Ice_InputStreamPtr self, Ice_Int* v);
void Ice_InputStream_readSize(Ice_InputStreamPtr self, Ice_Int* size, bool increment);
void Ice_InputStream_readString(Ice_InputStreamPtr self, char* v);
void Ice_InputStream_readIceString(Ice_InputStreamPtr self, Ice_StringPtr v);
void Ice_InputStream_readBlob(Ice_InputStreamPtr self, byte* v, int size);
void Ice_InputStream_readIdentity(Ice_InputStreamPtr self, char* name);

#ifdef _HAS_32BIT_FLOAT_
void Ice_InputStream_readFloat(Ice_InputStreamPtr self, Ice_Float* v);
#endif

typedef struct Ice_OutputStream {
    Object _base;

    byte data[MAX_MESSAGE_SIZE];
    byte* next;
    int size;
    byte* encaps[MAX_NESTED_ENCAPS];
    int current_encaps;
} Ice_OutputStream;

void Ice_OutputStream_init(Ice_OutputStreamPtr self);
void Ice_OutputStream_writeBool(Ice_OutputStreamPtr self, Ice_Bool v);
void Ice_OutputStream_writeByte(Ice_OutputStreamPtr self, Ice_Byte v);
void Ice_OutputStream_writeShort(Ice_OutputStreamPtr self, Ice_Short v);
void Ice_OutputStream_writeInt(Ice_OutputStreamPtr self, Ice_Int v);
void Ice_OutputStream_writeBlob(Ice_OutputStreamPtr self, const byte* data, int size);
void Ice_OutputStream_writeSize(Ice_OutputStreamPtr self, int size);
void Ice_OutputStream_writeString(Ice_OutputStreamPtr self, const char* str);
void Ice_OutputStream_writeIceString(Ice_OutputStreamPtr self, Ice_String str);
void Ice_OutputStream_writeIdentity(Ice_OutputStreamPtr self, const char* id);
void Ice_OutputStream_startWriteEncaps(Ice_OutputStreamPtr self);
void Ice_OutputStream_endWriteEncaps(Ice_OutputStreamPtr self);
void Ice_OutputStream_setMessageSize(Ice_OutputStreamPtr self);

#ifdef _HAS_32BIT_FLOAT_
void Ice_OutputStream_writeFloat(Ice_OutputStreamPtr self, Ice_Float v);
#endif

typedef struct {
    Object _base;

    Ice_EndpointType type;
    bool datagram;
    void* options;
} Ice_EndpointInfo;

typedef Ice_EndpointInfo* Ice_EndpointInfoPtr;

typedef struct {
    Object _base;

    Ice_EndpointInfoPtr epinfo;
    int fd;
    int current_client;
} Ice_Connection;

typedef Ice_Connection* Ice_ConnectionPtr;

void Ice_Connection_init(Ice_ConnectionPtr self, Ice_EndpointInfoPtr epinfo);
void Ice_Connection_send(Ice_ConnectionPtr self, byte* data, int size);
void Ice_Connection_sendValidate(Ice_ConnectionPtr self);
void Ice_Connection_receiveValidate(Ice_ConnectionPtr self);
void Ice_Connection_sendClose(Ice_ConnectionPtr self);

/* Forward declaration */
struct Ice_Communicator;
typedef struct Ice_Communicator* Ice_CommunicatorPtr;

typedef struct {
    Object _base;

    char ids[MAX_NUM_OF_SERVANTS][MAX_IDENTITY_SIZE];
    Ice_ObjectPtr objects[MAX_NUM_OF_SERVANTS];
    Ice_ObjectPtr default_servant;
    int count;
} Ice_ServantMap;

typedef Ice_ServantMap* Ice_ServantMapPtr;

void Ice_ServantMap_init(Ice_ServantMapPtr self);

typedef struct {
    Object _base;

    Ice_CommunicatorPtr communicator;
    Ice_Connection connection;
    bool activated;
    Ice_ServantMap servantMap;
    Ice_EndpointInfo epinfo;
} Ice_ObjectAdapter;

typedef Ice_ObjectAdapter* Ice_ObjectAdapterPtr;

void Ice_ObjectAdapter_init(Ice_ObjectAdapterPtr self,
                            Ice_CommunicatorPtr communicator);
void Ice_ObjectAdapter_activate(Ice_ObjectAdapterPtr self);
void Ice_ObjectAdapter_add(Ice_ObjectAdapterPtr self,
                           Ice_ObjectPtr servant,
                           const char* identity);
void Ice_ObjectAdapter_addDefaultServant(Ice_ObjectAdapterPtr self,
                                         Ice_ObjectPtr servant);
int Ice_ObjectAdapter_getClient(Ice_ObjectAdapterPtr self);
int Ice_ObjectAdapter_closeClient(Ice_ObjectAdapterPtr self);
void Ice_ObjectAdapter_incomingMessage(Ice_ObjectAdapterPtr self);

typedef struct {
    Object _base;
    Ice_ObjectAdapterPtr begin[MAX_NUM_OF_ADAPTERS];
    int count;
} Ice_ObjectAdapter_List;

typedef Ice_ObjectAdapter_List* Ice_ObjectAdapter_ListPtr;

void Ice_ObjectAdapter_List_init(Ice_ObjectAdapter_ListPtr self);
void IceInternal_selectOnAdapters(Ice_ObjectAdapter_ListPtr source,
                                  Ice_ObjectAdapterPtr result[],
                                  bool block);

typedef struct {
    Object _base;

    Ice_Connection connection;
    Ice_OutputStream stream;
    char identity[MAX_IDENTITY_SIZE];
    Ice_ReferenceMode mode;
    Ice_EndpointInfo epinfo;
} Ice_ObjectPrx;

typedef Ice_ObjectPrx* Ice_ObjectPrxPtr;

void Ice_ObjectPrx_connect(Ice_ObjectPrxPtr self);
void Ice_ObjectPrx_closeConnection(Ice_ObjectPrxPtr self);
void Ice_ObjectPrx_send(Ice_ObjectPrxPtr self);
void Ice_ObjectPrx_readFromInputStream(Ice_ObjectPrxPtr self, Ice_InputStreamPtr is);
void Ice_ObjectPrx_writeToOutputStream(Ice_ObjectPrxPtr self, Ice_OutputStreamPtr os);

void Ice_ObjectPrx_ice_identity(Ice_ObjectPrxPtr self, const char* newid);

typedef struct Ice_Communicator {
    Object _base;
    Ice_ObjectAdapter_List adapters;
} Ice_Communicator;

void Ice_Communicator_init(Ice_CommunicatorPtr self);
void Ice_Communicator_createObjectAdapterWithEndpoints(Ice_CommunicatorPtr self,
                                                       const char* name,
                                                       const char* endpoints,
                                                       Ice_ObjectAdapterPtr result);
void Ice_Communicator_registerAdapter(Ice_CommunicatorPtr self,
                                      Ice_ObjectAdapterPtr adapter);
void Ice_Communicator_stringToProxy(Ice_CommunicatorPtr self, const char* str,
                                    Ice_ObjectPrx* result);
void Ice_Communicator_waitForShutdown(Ice_CommunicatorPtr self);
void Ice_Communicator_loopIteration(Ice_CommunicatorPtr self);
void Ice_initialize(Ice_CommunicatorPtr ic);

typedef struct IcePlugin_PluginListItem* IcePlugin_PluginListItemPtr;

typedef struct IcePlugin_PluginListItem {
    Object _base;

    ObjectPtr object;
    IcePlugin_PluginListItemPtr next;
} IcePlugin_PluginListItem;

void IcePlugin_PluginListItem_init(IcePlugin_PluginListItemPtr self, ObjectPtr object);

typedef struct IcePlugin_EndpointObject {
    Object _base;

    bool (*getProtocolType)(const char*, Ice_EndpointType*);
    bool (*InputStream_init)(Ice_InputStreamPtr, int, Ice_EndpointType);
    bool (*ObjectAdapter_activate)(Ice_ObjectAdapterPtr);

    bool (*EndpointInfo_init)(Ice_EndpointInfoPtr, Ice_EndpointType, const char*);
    bool (*EndpointInfo_writeToOutputStream)(Ice_EndpointInfoPtr, Ice_OutputStreamPtr);
    bool (*EndpointInfo_readFromInputStream)(Ice_EndpointInfoPtr, Ice_InputStreamPtr);

    bool (*ObjectPrx_init)(Ice_ObjectPrxPtr, Ice_EndpointType, const char*);
    bool (*ObjectPrx_connect)(Ice_ObjectPrxPtr);

    bool (*Connection_accept)(Ice_ConnectionPtr);
    bool (*Connection_send)(Ice_ConnectionPtr, int, byte*, uint16_t*);
    bool (*Connection_close)(Ice_ConnectionPtr);
    bool (*Connection_dataReady)(Ice_ConnectionPtr, bool*);

} IcePlugin_EndpointObject;

typedef IcePlugin_EndpointObject* IcePlugin_EndpointObjectPtr;

void IcePlugin_registerEndpoint(IcePlugin_PluginListItemPtr item);
void IcePlugin_EndpointObject_init(IcePlugin_EndpointObjectPtr self);

void IcePlugin_getProtocolType(const char* proto, Ice_EndpointType* result);
void IcePlugin_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type);
void IcePlugin_Connection_accept(Ice_ConnectionPtr self);
int IcePlugin_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t size);
void IcePlugin_Connection_close(Ice_ConnectionPtr self);
void IcePlugin_Connection_dataReady(Ice_ConnectionPtr self, bool* result);
void IcePlugin_EndpointInfo_init(Ice_EndpointInfoPtr self,
                                 Ice_EndpointType type,
                                 const char* endpoint);
void IcePlugin_EndpointInfo_writeToOutputStream(Ice_EndpointInfoPtr self,
                                                Ice_OutputStreamPtr os);
void IcePlugin_EndpointInfo_readFromInputStream(Ice_EndpointInfoPtr self,
                                                Ice_InputStreamPtr is);

bool IcePlugin_ObjectAdapter_activate(Ice_ObjectAdapterPtr self);
void IcePlugin_ObjectPrx_init(Ice_ObjectPrxPtr self,
                              Ice_EndpointType type,
                              const char* strprx);
void IcePlugin_ObjectPrx_connect(Ice_ObjectPrxPtr self);

#ifdef __cplusplus
}
#endif

#endif  /* _ICE_ICEC_H_ */

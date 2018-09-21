/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceC.h>
#include <IceC/IceUtil.h>

typedef struct {
    Ice_String name;
    Ice_String category;
} Ice_Identity;

typedef Ice_Identity* Ice_IdentityPtr;

static void
Ice_Identity_writeToOutputStream(Ice_IdentityPtr self,
                                 Ice_OutputStreamPtr os) {

    trace();

    Ptr_check(os);

    Ice_OutputStream_writeIceString(os, self->name);
    Ice_OutputStream_writeIceString(os, self->category);
}

static void
Ice_Identity_readFromInputStream(Ice_IdentityPtr self,
                                 Ice_InputStreamPtr is) {

    trace();

    Ptr_check(is);

    Ice_InputStream_readIceString(is, &(self->name));
    Ice_InputStream_readIceString(is, &(self->category));
}

typedef struct {
    Ice_Identity key;
    Ice_ObjectPrx value;
} Ice_ObjectDict_Pair;

typedef Ice_ObjectDict_Pair* Ice_ObjectDict_PairPtr;

typedef struct {
    byte size;
    Ice_ObjectDict_PairPtr items;
} Ice_ObjectDict;

typedef Ice_ObjectDict* Ice_ObjectDictPtr;

#define Ice_ObjectDict_init Ice_Dict_init

#define Ice_ObjectDict_set(DICT, INDEX, KEY, VALUE)    \
do {                                                   \
    Ice_Dict_set(DICT.items[INDEX].key, KEY);          \
    Ice_Dict_set(DICT.items[INDEX].value, VALUE);      \
} while(0)

static void
Ice_ObjectDict_writeToOutputStream(Ice_ObjectDictPtr self,
                                   Ice_OutputStreamPtr os) {
    byte i = 0;

    trace();

    Ptr_check(os);

    Ice_OutputStream_writeSize(os, self->size);

    for (i=0; i<self->size; i++) {
        Ice_OutputStream_writeIceIdentity(os,
                (IceStd_IdentityPtr)&(self->items[i].key));
        Ice_ObjectPrx_writeToOutputStream(&(self->items[i].value), os);
    }
}

static void
Ice_ObjectDict_readFromInputStream(Ice_ObjectDictPtr self,
                                   Ice_InputStreamPtr is) {
    Ice_Int size = 0;
    byte i = 0;

    trace();

    Ptr_check(is);

    Ice_InputStream_readSize(is, &size, true);
    assert(size < 256);
    self->size = size;

    for (i=0; i<self->size; i++) {
        Ice_Identity_readFromInputStream(&(self->items[i].key), is);
        Ice_ObjectPrx_readFromInputStream(&(self->items[i].value), is);
    }
}

typedef struct {
    Ice_Int size;
    Ice_Identity* items;
} Ice_IdentitySeq;

typedef Ice_IdentitySeq* Ice_IdentitySeqPtr;

static void
Ice_IdentitySeq_writeToOutputStream(Ice_IdentitySeqPtr self,
                                    Ice_OutputStreamPtr os) {
    int i = 0;

    trace();

    Ptr_check(os);

    Ice_OutputStream_writeSize(os, self->size);

    for (i=0; i<self->size; i++) {
        Ice_OutputStream_writeIceIdentity(os,
                (IceStd_IdentityPtr)&(*(self->items + i)));
    }
}

static void
Ice_IdentitySeq_readFromInputStream(Ice_IdentitySeqPtr self,
                                    Ice_InputStreamPtr is) {
    int i = 0;

    trace();

    Ptr_check(is);

    Ice_InputStream_readSize(is, &(self->size), true);

    for (i=0; i<self->size; i++) {
        Ice_Identity_readFromInputStream(self->items + i, is);
    }
}

/* forward declaration */
struct Ice_Process;
typedef struct Ice_Process* Ice_ProcessPtr;
typedef Ice_ObjectPrx Ice_ProcessPrx;
typedef Ice_ObjectPrxPtr Ice_ProcessPrxPtr;

/* forward declaration */
struct Ice_LocatorRegistry;
typedef struct Ice_LocatorRegistry* Ice_LocatorRegistryPtr;
typedef Ice_ObjectPrx Ice_LocatorRegistryPrx;
typedef Ice_ObjectPrxPtr Ice_LocatorRegistryPrxPtr;

typedef struct {
    Ice_Object __base;
} Ice_Locator;

typedef Ice_Locator* Ice_LocatorPtr;

typedef Ice_ObjectPrx Ice_LocatorPrx;
typedef Ice_ObjectPrx* Ice_LocatorPrxPtr;

/* forward declaration */
static void
Ice_Locator_methodHandler(Ice_LocatorPtr self,
                          Ice_InputStreamPtr is,
                          Ice_OutputStreamPtr os);

static void
Ice_Locator_init(Ice_LocatorPtr self) {

    trace();

    Ice_Object_init((Ice_ObjectPtr)self,
            (Ice_MethodHandlerPtr)Ice_Locator_methodHandler);
}

void
Ice_LocatorI_findObjectById(Ice_LocatorPtr self,
                            Ice_Identity id,
                            Ice_ObjectPrxPtr retval) __attribute__((weak));

void
Ice_LocatorI_findAdapterById(Ice_LocatorPtr self,
                             Ice_String id,
                             Ice_ObjectPrxPtr retval) __attribute__((weak));

void
Ice_LocatorI_getRegistry(Ice_LocatorPtr self,
                         Ice_LocatorRegistryPrxPtr retval) __attribute__((weak));

static void
Ice_Locator_findObjectById__dispatch(Ice_LocatorPtr self,
                                     Ice_InputStreamPtr is,
                                     Ice_OutputStreamPtr os) {
    Ice_Identity id;
    Ice_ObjectPrx retval;

    trace();

    if (&Ice_LocatorI_findObjectById == 0) {
        return;
    }
    Ice_Identity_readFromInputStream(&id, is);

    Ice_LocatorI_findObjectById(self, id, &retval);
    Ice_ObjectPrx_writeToOutputStream(&(retval), os);
}

static void
Ice_Locator_findAdapterById__dispatch(Ice_LocatorPtr self,
                                      Ice_InputStreamPtr is,
                                      Ice_OutputStreamPtr os) {
    Ice_String id;
    Ice_ObjectPrx retval;

    trace();

    if (&Ice_LocatorI_findAdapterById == 0) {
        return;
    }
    Ice_InputStream_readIceString(is, &id);

    Ice_LocatorI_findAdapterById(self, id, &retval);
    Ice_ObjectPrx_writeToOutputStream(&(retval), os);
}

static void
Ice_Locator_getRegistry__dispatch(Ice_LocatorPtr self,
                                  Ice_InputStreamPtr is,
                                  Ice_OutputStreamPtr os) {
    Ice_LocatorRegistryPrx retval;

    trace();

    if (&Ice_LocatorI_getRegistry == 0) {
        return;
    }
    Ice_LocatorI_getRegistry(self, &retval);
    Ice_ObjectPrx_writeToOutputStream(&(retval), os);
}

static void
Ice_Locator_methodHandler(Ice_LocatorPtr self,
                          Ice_InputStreamPtr is,
                          Ice_OutputStreamPtr os) {
    Ice_Int operationSize = 0;
    char operation[MAX_OPERATION_NAME_SIZE];
    byte mode = 0;
    byte contextSize = 0;
    Ice_Int encapSize = 0;
    byte major = 0;
    byte minor = 0;

    trace();

    Ptr_check(self);
    Ptr_check(is);
    Ptr_check(os);

    Ice_InputStream_readSize(is, &operationSize, false);
    assert(operationSize <= MAX_OPERATION_NAME_SIZE);

    Ice_InputStream_readString(is, operation);

    /* FIXME: operationMode ignored */
    Ice_InputStream_readByte(is, &mode);

    /* FIXME: Context not supported */
    Ice_InputStream_readByte(is, &contextSize);
    assert(contextSize == 0);

    Ice_InputStream_readInt(is, &encapSize);
    Ice_InputStream_readByte(is, &major);
    Ice_InputStream_readByte(is, &minor);
    assert(major == 1 && minor == 0);
    assert(encapSize - 6 == (is->size - (is->next - is->data)));

    if (strcmp(operation, "findObjectById") == 0) {
        Ice_Locator_findObjectById__dispatch(self, is, os);
        return;
    }

    if (strcmp(operation, "findAdapterById") == 0) {
        Ice_Locator_findAdapterById__dispatch(self, is, os);
        return;
    }

    if (strcmp(operation, "getRegistry") == 0) {
        Ice_Locator_getRegistry__dispatch(self, is, os);
        return;
    }

    c_throw("Ice_OperationNotExistException", __FILE__, __LINE__);
}

static void
Ice_Locator_findObjectById(Ice_LocatorPrxPtr self,
                           Ice_Identity id,
                           Ice_ObjectPrxPtr retval) {
    Ice_OutputStreamPtr os = &(self->stream);

    trace();

    Ptr_check(self);

    Ice_ObjectPrx_connect(self);
    Ice_OutputStream_init(os);

    /* request header */
    Ice_OutputStream_writeBlob(os, Ice_header, sizeof(Ice_header));

    /* request body */
    if (self->mode == Ice_ReferenceModeTwoway) {
        Ice_OutputStream_writeInt(os, 1);
    }
    if (self->mode != Ice_ReferenceModeTwoway) {
        Ice_OutputStream_writeInt(os, 0);
    }

    Ice_OutputStream_writeIdentity(os, self->identity);
    Ice_OutputStream_writeString(os, "");
    Ice_OutputStream_writeString(os, "findObjectById");
    Ice_OutputStream_writeByte(os, Ice_Idempotent);
    Ice_OutputStream_writeByte(os, 0);

    /* encapsulated params */
    Ice_OutputStream_startWriteEncaps(os);
    Ice_OutputStream_writeIceIdentity(os, (IceStd_IdentityPtr)&(id));
    Ice_OutputStream_endWriteEncaps(os);
    Ice_OutputStream_setMessageSize(os);

    Ice_ObjectPrx_send(self);

    /* parse reply message (only on twoway invocations) */
    if (self->mode == Ice_ReferenceModeTwoway) {
        Ice_InputStream is;
        char expected[10] = {'I', 'c', 'e', 'P', 1, 0, 1, 0, Ice_replyMsg, 0};
        Ice_Int size = 0;
        Ice_Int requestId = 0;
        Ice_Byte replyStatus = 0;
        Ice_Int encapsSize = 0;
        Ice_Byte skipByte = 0;

        uint16_t timeout = 5000;
        bool ready = false;
        while (!ready) {
            IcePlugin_Connection_dataReady(&(self->connection), &ready);
            IceUtil_delay(1);
            if (! --timeout) {
                c_throw("Ice_TimeoutException", __FILE__, __LINE__);
                return;
            }
        }

        /* reply header */
        Ice_InputStream_init(&is, self->connection.fd, self->connection.epinfo->type);
        if (memcmp(expected, is.next, 10) != 0) {
            c_throw("Ice_ProtocolException", __FILE__, __LINE__);
            return;
        }

        /* skip header from input stream */
        is.next += 10;
        Ice_InputStream_readInt(&is, &size);
            if (size > MAX_MESSAGE_SIZE) {
            c_throw("Ice_MemoryLimitException", __FILE__, __LINE__);
            return;
        }

        /* reply body */
        Ice_InputStream_readInt(&is, &requestId);
            Ice_InputStream_readByte(&is, &replyStatus);

        /* check replyStatus */
        switch (replyStatus) {
        case 0: {
            /* success */
            break;
        }
        case 1: {
            /* user exception */
            Ice_ObjectPrx_parseUserException(self, &is);
            return;
        }
        case 2: {
            c_throw("Ice_ObjectDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 3: {
            c_throw("Ice_FacetDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 4: {
            c_throw("Ice_OperationDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 5:
        case 6:
        case 7: {
            /* unkown exception (reason as string) */
            /* FIXME: unmarshall and print reason */
            c_throw("Ice_UnknownException", __FILE__, __LINE__);
            return;
        }}

        Ice_InputStream_readInt(&is, &encapsSize);
            Ice_InputStream_readByte(&is, &skipByte);
            Ice_InputStream_readByte(&is, &skipByte);
            Ice_ObjectPrx_readFromInputStream(retval, &is);
        }

    Ice_ObjectPrx_closeConnection(self);
}

static void
Ice_Locator_findAdapterById(Ice_LocatorPrxPtr self,
                            Ice_String id,
                            Ice_ObjectPrxPtr retval) {
    Ice_OutputStreamPtr os = &(self->stream);

    trace();

    Ptr_check(self);

    Ice_ObjectPrx_connect(self);
    Ice_OutputStream_init(os);

    /* request header */
    Ice_OutputStream_writeBlob(os, Ice_header, sizeof(Ice_header));

    /* request body */
    if (self->mode == Ice_ReferenceModeTwoway) {
        Ice_OutputStream_writeInt(os, 1);
    }
    if (self->mode != Ice_ReferenceModeTwoway) {
        Ice_OutputStream_writeInt(os, 0);
    }

    Ice_OutputStream_writeIdentity(os, self->identity);
    Ice_OutputStream_writeString(os, "");
    Ice_OutputStream_writeString(os, "findAdapterById");
    Ice_OutputStream_writeByte(os, Ice_Idempotent);
    Ice_OutputStream_writeByte(os, 0);

    /* encapsulated params */
    Ice_OutputStream_startWriteEncaps(os);
    Ice_OutputStream_writeIceString(os, id);
    Ice_OutputStream_endWriteEncaps(os);
    Ice_OutputStream_setMessageSize(os);

    Ice_ObjectPrx_send(self);

    /* parse reply message (only on twoway invocations) */
    if (self->mode == Ice_ReferenceModeTwoway) {
        Ice_InputStream is;
        char expected[10] = {'I', 'c', 'e', 'P', 1, 0, 1, 0, Ice_replyMsg, 0};
        Ice_Int size = 0;
        Ice_Int requestId = 0;
        Ice_Byte replyStatus = 0;
        Ice_Int encapsSize = 0;
        Ice_Byte skipByte = 0;

        uint16_t timeout = 5000;
        bool ready = false;
        while (!ready) {
            IcePlugin_Connection_dataReady(&(self->connection), &ready);
            IceUtil_delay(1);
            if (! --timeout) {
                c_throw("Ice_TimeoutException", __FILE__, __LINE__);
                return;
            }
        }

        /* reply header */
        Ice_InputStream_init(&is, self->connection.fd, self->connection.epinfo->type);
        if (memcmp(expected, is.next, 10) != 0) {
            c_throw("Ice_ProtocolException", __FILE__, __LINE__);
            return;
        }

        /* skip header from input stream */
        is.next += 10;
        Ice_InputStream_readInt(&is, &size);
            if (size > MAX_MESSAGE_SIZE) {
            c_throw("Ice_MemoryLimitException", __FILE__, __LINE__);
            return;
        }

        /* reply body */
        Ice_InputStream_readInt(&is, &requestId);
            Ice_InputStream_readByte(&is, &replyStatus);

        /* check replyStatus */
        switch (replyStatus) {
        case 0: {
            /* success */
            break;
        }
        case 1: {
            /* user exception */
            Ice_ObjectPrx_parseUserException(self, &is);
            return;
        }
        case 2: {
            c_throw("Ice_ObjectDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 3: {
            c_throw("Ice_FacetDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 4: {
            c_throw("Ice_OperationDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 5:
        case 6:
        case 7: {
            /* unkown exception (reason as string) */
            /* FIXME: unmarshall and print reason */
            c_throw("Ice_UnknownException", __FILE__, __LINE__);
            return;
        }}

        Ice_InputStream_readInt(&is, &encapsSize);
            Ice_InputStream_readByte(&is, &skipByte);
            Ice_InputStream_readByte(&is, &skipByte);
            Ice_ObjectPrx_readFromInputStream(retval, &is);
        }

    Ice_ObjectPrx_closeConnection(self);
}

static void
Ice_Locator_getRegistry(Ice_LocatorPrxPtr self,
                        Ice_LocatorRegistryPrxPtr retval) {
    Ice_OutputStreamPtr os = &(self->stream);

    trace();

    Ptr_check(self);

    Ice_ObjectPrx_connect(self);
    Ice_OutputStream_init(os);

    /* request header */
    Ice_OutputStream_writeBlob(os, Ice_header, sizeof(Ice_header));

    /* request body */
    if (self->mode == Ice_ReferenceModeTwoway) {
        Ice_OutputStream_writeInt(os, 1);
    }
    if (self->mode != Ice_ReferenceModeTwoway) {
        Ice_OutputStream_writeInt(os, 0);
    }

    Ice_OutputStream_writeIdentity(os, self->identity);
    Ice_OutputStream_writeString(os, "");
    Ice_OutputStream_writeString(os, "getRegistry");
    Ice_OutputStream_writeByte(os, Ice_Idempotent);
    Ice_OutputStream_writeByte(os, 0);

    /* encapsulated params */
    Ice_OutputStream_startWriteEncaps(os);
    Ice_OutputStream_endWriteEncaps(os);
    Ice_OutputStream_setMessageSize(os);

    Ice_ObjectPrx_send(self);

    /* parse reply message (only on twoway invocations) */
    if (self->mode == Ice_ReferenceModeTwoway) {
        Ice_InputStream is;
        char expected[10] = {'I', 'c', 'e', 'P', 1, 0, 1, 0, Ice_replyMsg, 0};
        Ice_Int size = 0;
        Ice_Int requestId = 0;
        Ice_Byte replyStatus = 0;
        Ice_Int encapsSize = 0;
        Ice_Byte skipByte = 0;

        uint16_t timeout = 5000;
        bool ready = false;
        while (!ready) {
            IcePlugin_Connection_dataReady(&(self->connection), &ready);
            IceUtil_delay(1);
            if (! --timeout) {
                c_throw("Ice_TimeoutException", __FILE__, __LINE__);
                return;
            }
        }

        /* reply header */
        Ice_InputStream_init(&is, self->connection.fd, self->connection.epinfo->type);
        if (memcmp(expected, is.next, 10) != 0) {
            c_throw("Ice_ProtocolException", __FILE__, __LINE__);
            return;
        }

        /* skip header from input stream */
        is.next += 10;
        Ice_InputStream_readInt(&is, &size);
            if (size > MAX_MESSAGE_SIZE) {
            c_throw("Ice_MemoryLimitException", __FILE__, __LINE__);
            return;
        }

        /* reply body */
        Ice_InputStream_readInt(&is, &requestId);
            Ice_InputStream_readByte(&is, &replyStatus);

        /* check replyStatus */
        switch (replyStatus) {
        case 0: {
            /* success */
            break;
        }
        case 1: {
            /* user exception */
            Ice_ObjectPrx_parseUserException(self, &is);
            return;
        }
        case 2: {
            c_throw("Ice_ObjectDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 3: {
            c_throw("Ice_FacetDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 4: {
            c_throw("Ice_OperationDoesNotExist", __FILE__, __LINE__);
            return;
        }
        case 5:
        case 6:
        case 7: {
            /* unkown exception (reason as string) */
            /* FIXME: unmarshall and print reason */
            c_throw("Ice_UnknownException", __FILE__, __LINE__);
            return;
        }}

        Ice_InputStream_readInt(&is, &encapsSize);
            Ice_InputStream_readByte(&is, &skipByte);
            Ice_InputStream_readByte(&is, &skipByte);
            Ice_ObjectPrx_readFromInputStream(retval, &is);
        }

    Ice_ObjectPrx_closeConnection(self);
}

/* forward declaration */
struct Ice_LocatorFinder;
typedef struct Ice_LocatorFinder* Ice_LocatorFinderPtr;
typedef Ice_ObjectPrx Ice_LocatorFinderPrx;
typedef Ice_ObjectPrxPtr Ice_LocatorFinderPrxPtr;

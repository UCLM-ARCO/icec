/* -*- mode: c++; coding: utf-8 -*- */

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <IceC/IceUtil.h>
#include <IceC/platforms/esp8266/TCPEndpoint.h>

#ifndef MAX_TCP_ENDPOINT_INSTANCES
#define MAX_TCP_ENDPOINT_INSTANCES 5
#endif

#define TCP_PROTO_STRING "tcp"

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static TCP_EndpointOptions ep_options[MAX_TCP_ENDPOINT_INSTANCES];


bool ICEC_FUNC_ATTR
TCP_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (! streq(proto, TCP_PROTO_STRING))
        return false;

    *result = Ice_EndpointTypeTCP;
    return true;
}

bool ICEC_FUNC_ATTR
TCP_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) {
    Ice_Int size;

    trace();

    if (type != Ice_EndpointTypeTCP)
        return false;

    assert(fd != -1);

    // wait up to 5 seconds for data to arrive
    int timeout = 5000;
    WiFiClient* client = ep_options[fd].client;
    while (not client->available()) {
        delay(1);
        if (! --timeout)
            return false;
    }

    /* first: read header, and get message size */
    self->size = client->read((byte*)self->data, 14);
    size = (Ice_Int)*(self->data + 10);

    /* then: read as size specifies */
    size -= 14;
    assert(size <= MAX_MESSAGE_SIZE && "Message too big!");

    self->size += client->read((byte*)(self->data + 14), size);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    TCP_EndpointOptions* opts;

    trace();

    if (self->epinfo.type != Ice_EndpointTypeTCP)
        return false;

    opts = (TCP_EndpointOptions*)self->epinfo.options;
    Ptr_check(opts);

    return TCP_Connection_listen(&(self->connection), opts->host, opts->port);
}

void ICEC_FUNC_ATTR
_get_options_slot(TCP_EndpointOptionsPtr* options) {
    byte i;

    *options = NULL;
    for (i=0; i<MAX_TCP_ENDPOINT_INSTANCES; i++) {
        if (ep_options[i].used)
            continue;
        *options = &ep_options[i];
        break;
    }

    assert(*options != NULL && "Max number of endpoints reached!");
}

bool ICEC_FUNC_ATTR
TCP_EndpointInfo_init(Ice_EndpointInfoPtr self,
                      Ice_EndpointType type,
                      const char* endpoint) {

    trace();
    if (type != Ice_EndpointTypeTCP)
        return false;

    TCP_EndpointOptionsPtr options;
    _get_options_slot(&options);
    Ptr_check(options);

    self->type = type;
    self->datagram = false;
    self->options = options;
    options->used = true;

    IceUtil_getHost(endpoint, options->host);
    IceUtil_getPort(endpoint, &(options->port));
    Object_init((ObjectPtr)self);
    return true;
}

bool
TCP_EndpointInfo_writeToOutputStream(Ice_EndpointInfoPtr self,
                                     Ice_OutputStreamPtr os) {
    TCP_EndpointOptionsPtr opts;
    trace();

    Ptr_check(self);
    if (self->type != Ice_EndpointTypeTCP)
        return false;

    opts = (TCP_EndpointOptionsPtr)(self->options);
    Ptr_check(opts);
    Ptr_check(os);

    Ice_OutputStream_startWriteEncaps(os);
    Ice_OutputStream_writeString(os, opts->host);
    Ice_OutputStream_writeInt(os, opts->port);
    Ice_OutputStream_writeInt(os, opts->timeout);
    Ice_OutputStream_writeBool(os, false);
    Ice_OutputStream_endWriteEncaps(os);

    return true;
}

bool
TCP_EndpointInfo_readFromInputStream(Ice_EndpointInfoPtr self,
                                     Ice_InputStreamPtr is) {
    TCP_EndpointOptionsPtr opts;
    Ice_Int encapSize = 0;
    byte major = 0;
    byte minor = 0;
    trace();

    /* Note: type is set previously by IceC engine */
    if (self->type != Ice_EndpointTypeTCP)
        return false;

    _get_options_slot(&opts);
    self->options = opts;
    Ptr_check(opts);
    Ptr_check(is);

    self->datagram = false;

    /* read and check encapsulation header (note that this encap is
       nested, may be more data after it) */
    Ice_InputStream_readInt(is, &encapSize);
    Ice_InputStream_readByte(is, &major);
    Ice_InputStream_readByte(is, &minor);
    assert(major == 1 && minor == 0);
    assert(encapSize - 6 <= (is->size - (is->next - is->data)));

    /* read other endpoint params */
    Ice_InputStream_readString(is, opts->host);
    Ice_InputStream_readInt(is, (Ice_Int*)&(opts->port));
    Ice_InputStream_readInt(is, (Ice_Int*)&(opts->timeout));
    opts->used = true;

    /* Note: compression not supported */
    {
        bool comp;
        Ice_InputStream_readBool(is, &comp);
    }

    Object_init((ObjectPtr)self);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_EndpointInfo_toString(Ice_EndpointInfoPtr self, Ice_String* result) {
    byte i = 0;
    byte size;

    trace();

    // endpoint name
    const char* s = TCP_PROTO_STRING " -h ";

    size = strlen(s);
    if (size >= (result->size - i))
        size = result->size - i - 1;
    strncpy(result->value + i, s, size);
    i += size;

    // host
    TCP_EndpointOptionsPtr opts = (TCP_EndpointOptionsPtr)self->options;
    size = strlen(opts->host);
    if (size >= (result->size - i))
        size = result->size - i - 1;
    strncpy(result->value + i, opts->host, size);
    i += size;

    // port
    char* port = (char*)" -p      ";
    sprintf(port + 4, "%d", opts->port);
    size = strlen(port);
    if (size >= (result->size - i))
        size = result->size - i - 1;
    strncpy(result->value + i, port, size);
    i += size;

    // zero ending string
    *(result->value + i) = 0;
    return true;
}

bool ICEC_FUNC_ATTR
TCP_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    TCP_EndpointOptionsPtr opts;

    trace();
    Ptr_check(self);

    if (self->epinfo.type != Ice_EndpointTypeTCP)
        return false;

    opts = (TCP_EndpointOptionsPtr)self->epinfo.options;
    Ptr_check(opts);

    printf("[TCPEndpoint] connect to '%s':%d\n", opts->host, opts->port);
    TCP_Connection_connect(&(self->connection), opts->host, opts->port);
    assert(self->connection.fd != -1 && "Unable to connect!");
    return true;
}

bool ICEC_FUNC_ATTR
TCP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type,
                   const char* strprx) {
    trace();

    if (type != Ice_EndpointTypeTCP)
        return false;

    Ice_Connection_init(&(self->connection), &(self->epinfo));
    Ice_OutputStream_init(&(self->stream));
    TCP_EndpointInfo_init(&(self->epinfo), type, strprx);

    Object_init((ObjectPtr)self);
    return true;
}

void ICEC_FUNC_ATTR
TCP_Connection_connect(Ice_ConnectionPtr self, const char* host, uint16_t port) {
    TCP_EndpointOptionsPtr opts;

    trace();
    opts = (TCP_EndpointOptionsPtr)self->epinfo->options;
    Ptr_check(opts);

    assert((WiFi.getMode() == WIFI_AP || WiFi.status() == WL_CONNECTED) &&
           "WiFi not connected");
    assert(opts->client == NULL && "Already used endpoint");

    opts->client = new WiFiClient;
    bool result = opts->client->connect(host, port);
    assert(result && "Connection failed -1");
    self->fd = opts->fd;
}

bool ICEC_FUNC_ATTR
TCP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port) {
    TCP_EndpointOptionsPtr opts;

    trace();

    opts = (TCP_EndpointOptionsPtr)self->epinfo->options;
    Ptr_check(opts);

    assert((WiFi.getMode() == WIFI_AP || WiFi.status() == WL_CONNECTED) &&
           "WiFi not connected");
    assert(opts->server == NULL && "Already used endpoint");

    opts->server = new WiFiServer(port);
    opts->server->begin();
    return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_accept(Ice_ConnectionPtr self) {
    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
        return false;

    TCP_EndpointOptions* opts = (TCP_EndpointOptions*)self->epinfo->options;
    assert(opts->client == NULL && "Already connected client endpoint");

    opts->client = new WiFiClient(opts->server->available());
    if (opts->client) {
        self->fd = opts->fd;
        self->current_client = opts->fd;
    }

    return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t* size) {
    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
        return false;

    TCP_EndpointOptions* opts = (TCP_EndpointOptions*)self->epinfo->options;
    *size = opts->client->write((const uint8_t*)data, *size);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_close(Ice_ConnectionPtr self) {
    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
        return false;

    TCP_EndpointOptions* opts = (TCP_EndpointOptions*)self->epinfo->options;
    if (opts->client == NULL)
        return true;

    opts->client->stop();
    delete opts->client;
    opts->client = NULL;
    return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_dataReady(Ice_ConnectionPtr self, bool* result) {
    // trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
        return false;

    TCP_EndpointOptions* opts = (TCP_EndpointOptions*)self->epinfo->options;

    *result = false;
    if (opts->server != NULL) {
        *result = opts->server->hasClient();
    }
    else if (opts->client != NULL) {
        *result = opts->client->available();
    }
    return true;
}

void ICEC_FUNC_ATTR
TCPEndpoint_init(Ice_CommunicatorPtr ic) {
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    for (byte i=0; i<MAX_TCP_ENDPOINT_INSTANCES; i++) {
        ep_options[i].used = false;
        ep_options[i].fd = i;
        ep_options[i].server = NULL;
        ep_options[i].client = NULL;
        Object_init((ObjectPtr)&ep_options[i]);
    }

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

#ifdef ARDUINO_ARCH_ESP8266
    // get an (almost) ramdom number for source port
    int sport = ESP.getCycleCount() % 32000 + 32000;
    WiFiClient::setLocalPortStart(sport);
#endif
    endpoint.getProtocolType = &TCP_getProtocolType;
    endpoint.InputStream_init = &TCP_InputStream_init;
    endpoint.ObjectAdapter_activate = &TCP_ObjectAdapter_activate;

    endpoint.EndpointInfo_init = &TCP_EndpointInfo_init;
    endpoint.EndpointInfo_writeToOutputStream = &TCP_EndpointInfo_writeToOutputStream;
    endpoint.EndpointInfo_readFromInputStream = &TCP_EndpointInfo_readFromInputStream;
    endpoint.EndpointInfo_toString = &TCP_EndpointInfo_toString;

    endpoint.ObjectPrx_init = &TCP_ObjectPrx_init;
    endpoint.ObjectPrx_connect = &TCP_ObjectPrx_connect;

    endpoint.Connection_accept = &TCP_Connection_accept;
    endpoint.Connection_send = &TCP_Connection_send;
    endpoint.Connection_close = &TCP_Connection_close;
    endpoint.Connection_dataReady = &TCP_Connection_dataReady;
}

#endif /* ARDUINO_ARCH_ESP8266 || ARDUINO_ARCH_ESP32 */

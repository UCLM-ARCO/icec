/* -*- mode: c++; coding: utf-8 -*- */

#include <stdio.h>

#include "FreeRTOS.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOSIPConfig.h"

#include "IceUtil.h"
#include "TCPEndpoint.h"

#ifndef MAX_TCP_ENDPOINT_INSTANCES
#define MAX_TCP_ENDPOINT_INSTANCES 3
#endif

static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static TCP_EndpointOptions ep_options[MAX_TCP_ENDPOINT_INSTANCES];

/* Rx and Tx time outs are used to ensure the sockets do not wait too long for
missing data. */
static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 4000 );
static const TickType_t xSendTimeOut = pdMS_TO_TICKS( 2000 );


bool ICEC_FUNC_ATTR
TCP_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (! streq(proto, "tcp"))
    	return false;

    *result = Ice_EndpointTypeTCP;
    return true;
}

bool ICEC_FUNC_ATTR
TCP_InputStream_init(Ice_InputStreamPtr self, int fd, Ice_EndpointType type) {

    trace();

    if (type != Ice_EndpointTypeTCP)
	return false;

    Socket_t socket = ep_options[fd].connected_socket;
    self->size = FreeRTOS_recv(socket, (byte*)self->data, MAX_MESSAGE_SIZE, 0);
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

bool ICEC_FUNC_ATTR
TCP_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    TCP_EndpointOptionsPtr opts;

    trace();
    Ptr_check(self);

    if (self->epinfo.type != Ice_EndpointTypeTCP)
	return false;

    opts = (TCP_EndpointOptionsPtr)self->epinfo.options;
    Ptr_check(opts);

    TCP_Connection_connect(&(self->connection), opts->host, opts->port);
    assert(self->connection.fd != -1);
    return true;
}

bool ICEC_FUNC_ATTR
TCP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx) {
    trace();

    if (type != Ice_EndpointTypeTCP)
	return false;

    Ice_Connection_init(&(self->connection), &(self->epinfo));
    Ice_OutputStream_init(&(self->stream));
    TCP_EndpointInfo_init(&(self->epinfo), type, strprx);

    Object_init((ObjectPtr)self);
    return true;
}

void open_freertos_socket(Ice_ConnectionPtr self)
{
	Socket_t socket;
	TCP_EndpointOptionsPtr opts;

	#if( ipconfigUSE_TCP_WIN == 1 )
	WinProperties_t xWinProps;

	/* Fill in the buffer and window sizes that will be used by the socket. */
	xWinProps.lTxBufSize = ipconfigTCP_TX_BUFFER_LENGTH;
	xWinProps.lTxWinSize = ipconfigTCP_TX_BUFFER_LENGTH * 2;
	xWinProps.lRxBufSize = ipconfigTCP_RX_BUFFER_LENGTH;
	xWinProps.lRxWinSize = ipconfigTCP_RX_BUFFER_LENGTH * 2;
	#endif /* ipconfigUSE_TCP_WIN */

	trace();

	opts = (TCP_EndpointOptionsPtr)self->epinfo->options;
	Ptr_check(opts);

	socket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );
	if (socket == FREERTOS_INVALID_SOCKET) {
		FreeRTOS_printf( ("Could not create socket\n" ));
		return;
	}

	/* set timeouts so the tasks doesn't block indefinitely */
	FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
	FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );

	#if( ipconfigUSE_TCP_WIN == 1 )
	{
		/* Set the window and buffer sizes. */
		FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_WIN_PROPERTIES, ( void * ) &xWinProps,	sizeof( xWinProps ) );
	}
	#endif /* ipconfigUSE_TCP_WIN */

	self->fd = opts->fd; // fd will be used as the index for locating the EP options
	opts->socket = socket;
}

int inet_aton(const char* host, uint32_t *addr) {
	unsigned char ip[4];
	int scan_result;

	scan_result = sscanf(host, "%hhu.%hhu.%hhu.%hhu", &ip[3], &ip[2], &ip[1], &ip[0]);
	if (scan_result != 4)
	FreeRTOS_inet_addr_quick(ip[0], ip[1], ip[2], ip[3]);
	return true;
}

void ICEC_FUNC_ATTR
TCP_Connection_connect(Ice_ConnectionPtr self, const char* host, uint16_t port) {
	Socket_t socket = NULL;
    struct freertos_sockaddr serverAddress;

    open_freertos_socket(self);
    socket = ((TCP_EndpointOptionsPtr)self->epinfo->options)->socket;

	serverAddress.sin_port = FreeRTOS_htons( port );
	if (!inet_aton(host, &serverAddress.sin_addr)) {
		FreeRTOS_printf( ("Invalid host name: %s\n", serverAddress.sin_addr ));
		return;
	}

	if( FreeRTOS_connect( socket, &serverAddress, sizeof( serverAddress ) ) != 0 ) {
		FreeRTOS_printf( ("Socket failed to connect\n" ));
		return;
	}
}

bool ICEC_FUNC_ATTR
TCP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port) {
    Socket_t socket = NULL;
    struct freertos_sockaddr serverAddress;
    const BaseType_t xBacklog = 20;

    open_freertos_socket(self);
    socket = ((TCP_EndpointOptionsPtr)self->epinfo->options)->socket;

	serverAddress.sin_port = FreeRTOS_htons( port );
	if( FreeRTOS_bind( socket, &serverAddress, sizeof( serverAddress ) ) != 0 ) {
		FreeRTOS_printf( ("Could not bind socket") );
		return false;
	}

	if( FreeRTOS_listen( socket, xBacklog )  != 0 ) {
		FreeRTOS_printf( ("Could not listen to socket") );
		return false;
	}

    return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_accept(Ice_ConnectionPtr self) {
	struct freertos_sockaddr xClient;
	socklen_t xSize = sizeof( xClient );

    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;

    TCP_EndpointOptions* opts = (TCP_EndpointOptions*)self->epinfo->options;

	opts->connected_socket = FreeRTOS_accept( opts->socket, &xClient, &xSize );
	if( opts->connected_socket == NULL ) {
		FreeRTOS_printf( ("Invalid socket in accept") );
		return false;
	}
    self->current_client = opts->fd;
    self->fd = opts->fd;

	/* set timeouts so the tasks doesn't block indefinitely */
	FreeRTOS_setsockopt( opts->connected_socket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
	FreeRTOS_setsockopt( opts->connected_socket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );

    return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_send(Ice_ConnectionPtr self, int fd, byte* data, uint16_t* size) {
    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;


    TCP_EndpointOptions* opts = (TCP_EndpointOptions*)self->epinfo->options;
	*size = FreeRTOS_send( opts->connected_socket, data, *size, 0 );

	return true;
}

bool ICEC_FUNC_ATTR
TCP_Connection_close(Ice_ConnectionPtr self) {
    trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;

    TCP_EndpointOptions* opts = (TCP_EndpointOptions*)self->epinfo->options;
    if (opts->connected_socket == NULL)
    	return true;

	FreeRTOS_closesocket( opts->connected_socket );
    return true;
}


/*
bool ICEC_FUNC_ATTR
TCP_Connection_dataReady(Ice_ConnectionPtr self, bool* result) {
     trace();

    if (self->epinfo->type != Ice_EndpointTypeTCP)
	return false;

    return true;
}
*/

void ICEC_FUNC_ATTR
TCPEndpoint_init(Ice_CommunicatorPtr ic) {
	byte i;
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    for (i=0; i<MAX_TCP_ENDPOINT_INSTANCES; i++) {
	ep_options[i].used = false;
	ep_options[i].fd = i;
	ep_options[i].socket = NULL;
	ep_options[i].connected_socket = NULL;
	Object_init((ObjectPtr)&ep_options[i]);
    }

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

    endpoint.getProtocolType = &TCP_getProtocolType;
    endpoint.InputStream_init = &TCP_InputStream_init;
    endpoint.ObjectAdapter_activate = &TCP_ObjectAdapter_activate;

    endpoint.EndpointInfo_init = &TCP_EndpointInfo_init;
    endpoint.EndpointInfo_writeToOutputStream = NULL;
    endpoint.EndpointInfo_readFromInputStream = NULL;

    endpoint.ObjectPrx_init = &TCP_ObjectPrx_init;
    endpoint.ObjectPrx_connect = &TCP_ObjectPrx_connect;

    endpoint.Connection_accept = &TCP_Connection_accept;
    endpoint.Connection_send = &TCP_Connection_send;
    endpoint.Connection_close = &TCP_Connection_close;
    //endpoint.Connection_dataReady = &TCP_Connection_dataReady;
}

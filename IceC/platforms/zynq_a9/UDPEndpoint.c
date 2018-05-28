#include <stdio.h>

#include "FreeRTOS.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOSIPConfig.h"

#include <IceC/IceUtil.h>
#include <IceC/platforms/zynq_a9/UDPEndpoint.h>

/* NOTE: only one endpoint instance allowed */
static IcePlugin_PluginListItem item;
static IcePlugin_EndpointObject endpoint;
static UDP_EndpointOptions ep_options;

static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 4000 );
static const TickType_t xSendTimeOut = pdMS_TO_TICKS( 2000 );

bool
UDP_getProtocolType(const char* proto, Ice_EndpointType* result) {
    trace();

    if (! streq(proto, "udp"))
    	return false;

    *result = Ice_EndpointTypeUDP;
    return true;
}

bool
UDP_InputStream_init(Ice_InputStreamPtr self, Ice_ConnectionPtr connection, Ice_EndpointType type) {
    trace();
    
    if (type != Ice_EndpointTypeUDP)
	return false;
    
    UDP_EndpointOptionsPtr opts = connection->epinfo->options;
    Socket_t socket = opts->socket;
    struct freertos_sockaddr xSourceAddress;    
    self->size = FreeRTOS_recvfrom(socket, (byte*)self->data, MAX_MESSAGE_SIZE, 0,
				   &xSourceAddress, sizeof(xSourceAddress));
    return true;
}

bool
UDP_ObjectAdapter_activate(Ice_ObjectAdapterPtr self) {
    UDP_EndpointOptionsPtr opts;

    trace();

    if (self->epinfo.type != Ice_EndpointTypeUDP)
	return false;

    opts = (UDP_EndpointOptions*)self->epinfo.options;
    Ptr_check(opts);

    return UDP_Connection_listen(&(self->connection), opts->host, opts->port);
}

bool
UDP_EndpointInfo_init(Ice_EndpointInfoPtr self,
		      Ice_EndpointType type,
		      const char* endpoint) {

    ObjectPtr _ep_options_ptr;

    trace();
    if (type != Ice_EndpointTypeUDP)
	return false;

    /* avoid annoying compiler messages */
    _ep_options_ptr = (ObjectPtr)&ep_options;
    Ptr_check(_ep_options_ptr);

    assert(! ep_options.used && "Only one instance of this endpoint allowed!");

    self->type = type;
    self->datagram = true;
    self->options = &ep_options;
    ep_options.used = true;

    IceUtil_getHost(endpoint, ep_options.host);
    IceUtil_getPort(endpoint, &(ep_options.port));
    Object_init((ObjectPtr)self);
    return true;
}

bool
UDP_ObjectPrx_init(Ice_ObjectPrxPtr self, Ice_EndpointType type, const char* strprx,
		   IcePlugin_EndpointObjectPtr endp) {
    ObjectPtr _ep_options_ptr;

    trace();
    if (type != Ice_EndpointTypeUDP)
	return false;

    /* avoid annoying compiler messages */
    _ep_options_ptr = (ObjectPtr)&ep_options;
    Ptr_check(_ep_options_ptr);

    assert(! ep_options.used && "Only one instance of this endpoint allowed!");

    Ice_Connection_init(&(self->connection), &(self->epinfo), endp);
    Ice_OutputStream_init(&(self->stream));
    UDP_EndpointInfo_init(&(self->epinfo), type, strprx);

    self->mode = Ice_ReferenceModeDatagram;
    Object_init((ObjectPtr)self);
    return true;
}

bool open_freertos_socket(Ice_ConnectionPtr self)
{
    Socket_t socket;
    UDP_EndpointOptionsPtr opts;
  
    trace();

    opts = (UDP_EndpointOptionsPtr)self->epinfo->options;
    Ptr_check(opts);

    socket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
    if (socket == FREERTOS_INVALID_SOCKET) {
      FreeRTOS_printf( ("Could not create socket\n" ));
      return false;
    }

    /* set timeouts so the tasks doesn't block indefinitely */
    FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
    FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );

    //self->fd = 0;//opts->fd; // fd will be used as the index for locating the EP options
    opts->socket = socket;
    return true;
}

int inet_aton(const char* host, uint32_t *addr) {
	unsigned char ip[4];
	int scan_result;

	scan_result = sscanf(host, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (scan_result != 4)
		return false;

	FreeRTOS_inet_addr_quick(ip[0], ip[1], ip[2], ip[3]);
	return true;
}

bool
UDP_ObjectPrx_connect(Ice_ObjectPrxPtr self) {
    Ice_ConnectionPtr conn;
    UDP_EndpointOptionsPtr opts;
    struct freertos_sockaddr serverAddress;

    trace();
    if (self->epinfo.type != Ice_EndpointTypeUDP)
	return false;

    conn = &(self->connection);
    Ptr_check(conn);
    opts = self->epinfo.options;
    Ptr_check(opts);

    serverAddress.sin_port = FreeRTOS_htons( opts->port );
    if (!inet_aton(opts->host, &serverAddress.sin_addr)) {
      FreeRTOS_printf( ("Invalid host name: %s\n", serverAddress.sin_addr ));
      return false;
    }
    
    return open_freertos_socket(conn);

}

bool
UDP_Connection_listen(Ice_ConnectionPtr self, const char* host, uint16_t port) {
    Socket_t socket = NULL;
    struct freertos_sockaddr serverAddress;
    
    if (!open_freertos_socket(self))
    	return false;

    socket = ((UDP_EndpointOptionsPtr)self->epinfo->options)->socket;

    serverAddress.sin_port = FreeRTOS_htons( port );
    if( FreeRTOS_bind( socket, &serverAddress, sizeof( serverAddress ) ) != 0 ) {
      FreeRTOS_printf( ("Could not bind socket") );
      return false;
    }   

    ((UDP_EndpointOptionsPtr)self->epinfo->options)->socket_set = FreeRTOS_CreateSocketSet();    
    return true;
}


/*FIXME: dst and port*/
bool
UDP_Connection_send(Ice_ConnectionPtr self, byte* data, uint16_t* size) {
    struct freertos_sockaddr xDestinationAddress;
    int32_t isize_send;
    trace();
    
    if (self->epinfo->type != Ice_EndpointTypeUDP)
	return false;

    UDP_EndpointOptions* opts = (UDP_EndpointOptions*)self->epinfo->options;

    xDestinationAddress.sin_addr = FreeRTOS_inet_addr( opts->host );
    xDestinationAddress.sin_port = FreeRTOS_htons( opts->port );
    
    //*size = FreeRTOS_send( opts->socket, data, *size, 0 );
    isize_send = FreeRTOS_sendto( opts->socket, data, *size, 0, &xDestinationAddress, sizeof(xDestinationAddress) );

    if(isize_send != *size){
      FreeRTOS_printf( ("Could not send the message") );
      return false;
    }
    
    return true;
}


void
UDPEndpoint_init(Ice_CommunicatorPtr ic) {
    trace();

    /* Note: is important to call IcePlugin_registerEndpoint after
       Ice_Communicator_initialize, check here that communicator is
       initialized. */
    Ptr_check(ic);

    ep_options.used = false;
    ep_options.socket = NULL;
    ep_options.connected_socket = NULL;
    Object_init((ObjectPtr)&ep_options);

    IcePlugin_EndpointObject_init(&endpoint);
    IcePlugin_PluginListItem_init(&item, (ObjectPtr)&endpoint);
    IcePlugin_registerEndpoint(&item);

    endpoint.getProtocolType = &UDP_getProtocolType;
    endpoint.InputStream_init = &UDP_InputStream_init;
    endpoint.ObjectAdapter_activate = &UDP_ObjectAdapter_activate;
    
    endpoint.EndpointInfo_init = &UDP_EndpointInfo_init;
    endpoint.EndpointInfo_writeToOutputStream = NULL;  /* FIXME: not implemented */
    endpoint.EndpointInfo_readFromInputStream = NULL;  /* FIXME: not implemented */
    
    endpoint.ObjectPrx_init = &UDP_ObjectPrx_init;
    endpoint.ObjectPrx_connect = &UDP_ObjectPrx_connect;
    
    endpoint.Connection_accept = NULL;
    endpoint.Connection_send = &UDP_Connection_send;
    endpoint.Connection_close = NULL;
    endpoint.Connection_dataReady = NULL;  /* FIXME: not implemented */
}

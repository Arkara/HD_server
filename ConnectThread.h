#ifndef _HD_ConnectThread

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <mutex>




#include "SocketThread.h"






class ConnectThread
{
private :
    std::thread     ConnectionListenerThread;
    SocketThreads*  SocketHandlerThreads;
    int             status;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    int             SocketDescriptor;

    void InitHostInfo();
    void InitReceiveSocket();
    void InitPort();
    void BindSocket();
    void ReceiveConnectionAttempts();
 
    enum SocketStateIDs { 
         DISCONNECTED,
         GETADDRINFO_INIT,
         SOCKET_INIT,
         PORT_INIT,
         BIND_INIT,
         LISTENING
    } SocketState = GETADDRINFO_INIT;

public :
    ConnectThread( SocketThreads* SocketHandlerThreads );
    ~ConnectThread();
    void ConnectionListener();


};


#define _HD_ConnectThread
#endif




#include "ConnectThread.h"
#include <iostream>




void ConnectThread::InitHostInfo()
{
printf( "ConnectThread::InitHostInfo\n" );
    memset(&host_info, 0, sizeof host_info);

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

    status = getaddrinfo(NULL, "56124", &host_info, &host_info_list);
    if (status != 0)
    {
//        SocketState = GETADDRINFO_ERROR;
        printf("getaddrinfo error %s\n", gai_strerror(status));
    }
    else
    {
        SocketState = SOCKET_INIT;
    }
}

void ConnectThread::InitReceiveSocket()
{
printf( "ConnectThread::InitReceiveSocket\n" );
    SocketDescriptor = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (SocketDescriptor == -1)
    {
//        SocketState = SOCKET_ERROR;
        printf("socket error\n");
        return;
    }
    else
    {
        SocketState = PORT_INIT;
    }
}

void ConnectThread::InitPort()
{
printf( "ConnectThread::InitPort\n" );
    // make sure the port is not in use from previous execution of our code.
    int yes = 1;
    status = setsockopt(SocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1)
    {
//        SocketState = PORT_ERROR;
        printf("setsockopt error\n");
    }
    else
    {
        SocketState = BIND_INIT;
    }
}

void ConnectThread::BindSocket()
{
printf( "ConnectThread::BindSocket\n" );
    status = bind(SocketDescriptor, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)
    {
//        SocketState = BIND_ERROR;
        printf("bind error\n");
    }
    else
    {
        SocketState = LISTENING;
    }
}

void ConnectThread::ReceiveConnectionAttempts()
{
printf( "ConnectThread::ReceiveConnectionAttempts\n" );
    status =  listen(SocketDescriptor, 5);
    if (status == -1)
    {
//        SocketState = LISTEN_ERROR;
        printf("listen error\n");
        return;
    }

    int                       new_sd;
    struct sockaddr_storage   their_addr;
    socklen_t                 addr_size = sizeof(their_addr);

    new_sd = accept(SocketDescriptor, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
    {
        printf( "listen error \n");
        return;
    }
    else
    {
       printf("Connection accepted. Using new SocketDescriptor : %i\n", new_sd);
    }
}

void ConnectThread::ConnectionListener() {
printf( "ConnectThread::ConnectionListener, initial state = %i\n", SocketState );

    while(true)
    {
        if( SocketState == LISTENING )
        {
            ReceiveConnectionAttempts();
        }
        else if( SocketState ==  GETADDRINFO_INIT )
        {
            InitHostInfo();
        }
        else if( SocketState == SOCKET_INIT )
        {
            InitReceiveSocket();
        }
        else if( SocketState == PORT_INIT )
        {
            InitPort();
        }
        else if( SocketState == BIND_INIT )
        {
            BindSocket();
        }
    }

}

ConnectThread::ConnectThread(  SocketThreads* pSocketHandlerThreads )
{
    SocketHandlerThreads=pSocketHandlerThreads;
    ConnectionListenerThread = std::thread([=] { ConnectionListener(); });

}

ConnectThread::~ConnectThread()
{
    if( host_info_list!=NULL )
    {
        freeaddrinfo(host_info_list);
    }
    ConnectionListenerThread.join();
}

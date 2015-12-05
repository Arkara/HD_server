#include "ConnectThread.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <netinet/tcp.h>





void ConnectThread::InitHostInfo(const char *TargetHostURL)
{
printf( "ConnectThread::InitHostInfo\n" );
    memset(&host_info, 0, sizeof host_info);

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

    if( host_info_list!=NULL )
    {
        freeaddrinfo(host_info_list);
        host_info_list = NULL;
    }
    status = getaddrinfo(TargetHostURL, "56124", &host_info, &host_info_list);
    if (status != 0)
    {
        printf("getaddrinfo error %s\n", gai_strerror(status));
    }
    else
    {
        SocketState = Connect_Thread_SOCKET_INIT;
    }
}

void ConnectThread::InitReceiveSocket()
{
printf( "ConnectThread::InitReceiveSocket\n" );
    SocketDescriptor = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (SocketDescriptor == -1)
    {
        printf("socket error\n");
        SocketState = Connect_Thread_GETADDRINFO_INIT;
    }
    else
    {
        SocketState = Connect_Thread_SOCKET_CONFIG;
    }
}

void ConnectThread::InitSocketConfig()
{
printf( "ConnectThread::InitSocketConfig\n" );
    int    on = 1;
 
    status = ioctl(SocketDescriptor, FIONBIO, (char *)&on);
    if (status < 0)
    {
printf( "ConnectThread::InitSocketConfig failed to set FIONBIO\n" );
        shutdown( SocketDescriptor, SHUT_RDWR);
        SocketDescriptor = -1;
        SocketState = Connect_Thread_GETADDRINFO_INIT;
        return;
    }

    //status = ioctl(SocketDescriptor, TCP_NODELAY, (char *)&on);
    status = setsockopt(SocketDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(int)); 
    if (status < 0)
    {
printf( "ConnectThread::InitSocketConfig failed to set TCP_NODELAY\n" );
        shutdown( SocketDescriptor, SHUT_RDWR);
        SocketDescriptor = -1;
        SocketState = Connect_Thread_GETADDRINFO_INIT;
        return;
    }

    SocketState = Connect_Thread_PORT_INIT;
}

void ConnectThread::InitPort()
{
printf( "ConnectThread::InitPort\n" );
    // make sure the port is not in use from previous execution of our code.
    int yes = 1;
    status = setsockopt(SocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1)
    {
        printf("setsockopt error\n");
        shutdown( SocketDescriptor, SHUT_RDWR);
        SocketDescriptor = -1;
        SocketState = Connect_Thread_GETADDRINFO_INIT;
    }
    else
    {
        SocketState = Connect_Thread_BIND_INIT;
    }
}

void ConnectThread::BindSocket()
{
printf( "ConnectThread::BindSocket\n" );
    status = bind(SocketDescriptor, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)
    {
        printf("bind error\n");
        shutdown( SocketDescriptor, SHUT_RDWR);
        SocketDescriptor = -1;
        SocketState = Connect_Thread_GETADDRINFO_INIT;
    }
    else
    {
        SocketState = Connect_Thread_LISTENING;
    }
}

void ConnectThread::ReceiveConnectionAttempts()
{
//printf( "ConnectThread::ReceiveConnectionAttempts\n" );  //this commented out so avoid continuous prints to console.
    status =  listen(SocketDescriptor, 256 );
    if (status == -1)
    {
        printf("listen error\n");
        return;
    }

    int AcceptStatus = 0;
    while( AcceptStatus!= EWOULDBLOCK )
    {

        int                       new_sd;
        struct sockaddr_storage   their_addr;
        socklen_t                 addr_size = sizeof(their_addr);

        new_sd = accept(SocketDescriptor, (struct sockaddr *)&their_addr, &addr_size);
        if (new_sd < 0 )
        {
            if( errno == EWOULDBLOCK )
            {
                AcceptStatus = EWOULDBLOCK;
            }
            else
            {
                printf( "listen error \n");
                return;
            }
        }
        else
        {
            printf("Connection accepted. Using new SocketDescriptor : %i\n", new_sd);
            SocketLeader.AddSocketEntityToQueue(SocketEntity(new_sd));
        }
    }
}

void ConnectThread::ConnectionListener()
{
printf( "ConnectThread::ConnectionListener\n");

    while(SocketState!=Connect_Thread_TERMINATE)
    {
        if( SocketState == Connect_Thread_LISTENING )
        {
            ReceiveConnectionAttempts();
        }
        else if( SocketState ==  Connect_Thread_GETADDRINFO_INIT )
        {
            InitHostInfo(NULL);
        }
        else if( SocketState == Connect_Thread_SOCKET_INIT )
        {
            InitReceiveSocket();
        }
        else if( SocketState == Connect_Thread_SOCKET_CONFIG )
        {
            InitSocketConfig();
        }
        else if( SocketState == Connect_Thread_PORT_INIT )
        {
            InitPort();
        }
        else if( SocketState == Connect_Thread_BIND_INIT )
        {
            BindSocket();
        }

       std::this_thread::sleep_for(std::chrono::milliseconds(ConnectionPollingDelay));
    }
    

}

ConnectThread::ConnectThread()
{
    SocketState = Connect_Thread_GETADDRINFO_INIT;
    ConnectionListenerThread = std::thread([=] { ConnectionListener(); });

}

ConnectThread::~ConnectThread()
{
printf( "ConnectThread::~ConnectThread begins\n");

    SocketState = Connect_Thread_TERMINATE;

    if( host_info_list!=NULL )
    {
        freeaddrinfo(host_info_list);
    }
    ConnectionListenerThread.join();
printf( "ConnectThread::~ConnectThread completed\n");
}

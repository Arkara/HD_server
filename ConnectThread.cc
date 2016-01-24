//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon


#include <iostream>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>





#include "ConnectThread.h"


void ConnectThread::ResetSocket()
{
    shutdown( SocketDescriptor, SHUT_RDWR);
    SocketDescriptor = -1;
    SocketState = Connect_Thread_ADDRINFO_INIT;
}

void ConnectThread::InitHostInfo()
{
//fprintf( stderr, "ConnectThread::InitHostInfo\n" );
    memset(&host_info, 0, sizeof host_info);

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

    if( host_info_list!=NULL )
    {
        freeaddrinfo(host_info_list);
        host_info_list = NULL;
    }
    SocketState = Connect_Thread_ADDRINFO_INIT;
}

void ConnectThread::InitAddressInfo(const char *TargetHostURL)
{
//fprintf( stderr, "ConnectThread::InitHostInfo\n" );
    status = getaddrinfo(TargetHostURL, "56124", &host_info, &host_info_list);
    if (status != 0)
    {
        printf("getaddrinfo error %s\n", gai_strerror(status));
        SocketState = Connect_Thread_HOSTINFO_INIT;
    }
    else
    {
        SocketState = Connect_Thread_SOCKET_INIT;
    }
}

void ConnectThread::InitReceiveSocket()
{
//fprintf( stderr, "ConnectThread::InitReceiveSocket\n" );
    SocketDescriptor = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (SocketDescriptor == -1)
    {

        /* SOCKET level errors */
        if( errno==EACCES )
        {
            LogMessage( LOG_EMERG, "ConnectThread::InitReceiveSocket", "EACCESS error: Permission to create listener socket is denied; Awaiting O/S and Console Intervention." );
            SocketState=Connect_Thread_SUSPEND_FOR_CONSOLE_INTERVENTION;
            return;
        }
        if( errno==EAFNOSUPPORT )
        {
            LogMessage( LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EAFNOSUPPORT error : The implementation does not support the specified address family; Terminating Server." );
            SocketState=TERMINATE_SERVER;
            return;
        }
        if( errno==EINVAL )
        {
            LogMessage( LOG_EMERG, "ConnectThread::InitReceiveSocket", "EINVAL error : Unknown protocol, or protocol family not available; Awaiting O/S and Console Intervention" );
            SocketState=Connect_Thread_SUSPEND_FOR_CONSOLE_INTERVENTION;
            return;
        }
        if( errno==EMFILE )
        {
            LogMessage( LOG_ERR, "ConnectThread::InitReceiveSocket", "EMFILE error : The per-process limit on the number of open file descriptors has been reached; Will Retry." );
            std::this_thread::sleep_for(std::chrono::milliseconds(InitReceiveSocketRetryDelay));
            SocketState = Connect_Thread_SOCKET_INIT;
            return;
        }
        if( errno==ENFILE )
        {
            LogMessage( LOG_ERR, "ConnectThread::InitReceiveSocket", "ENFILE error : The system-wide limit on the total number of open files has been reached; Will Retry." );
            std::this_thread::sleep_for(std::chrono::milliseconds(InitReceiveSocketRetryDelay));
            SocketState = Connect_Thread_SOCKET_INIT;
            return;
        }
        if( errno==ENOBUFS )
        {
            LogMessage( LOG_ERR, "ConnectThread::InitReceiveSocket", "ENOBUFS error : The socket cannot be created until sufficient resources are freed; Will Retry" );
            std::this_thread::sleep_for(std::chrono::milliseconds(InitReceiveSocketRetryDelay));
            SocketState = Connect_Thread_SOCKET_INIT;
            return;
        }
        if( errno==ENOMEM )
        {
            LogMessage( LOG_ERR, "ConnectThread::InitReceiveSocket", "ENOMEM error : The socket cannot be created until sufficient resources are freed; Will Retry" );
            std::this_thread::sleep_for(std::chrono::milliseconds(InitReceiveSocketRetryDelay));
            SocketState = Connect_Thread_SOCKET_INIT;
            return;
        }
        if( errno==EPROTONOSUPPORT  )
        {
            LogMessage( LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EPROTONOSUPPORT error : The protocol type or the specified protocol is not supported within this domain; Terminating Server" );
            SocketState=TERMINATE_SERVER;
            return;
        }

        /* IPV4 level errors */
        if( errno==EADDRINUSE )
        {
            LogMessage( LOG_EMERG, "ConnectThread::InitReceiveSocket", "EADDRINUSE error :  Tried to bind to an address already in use; Awaiting O/S and Console Intervention" );
            SocketState=Connect_Thread_SUSPEND_FOR_CONSOLE_INTERVENTION;
            return;
        }

        if( errno==EADDRNOTAVAIL  )
        {
            LogMessage( LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EADDRNOTAVAIL error :  A nonexistent interface was requested or the requested source address was not local; Terminating Server" );
            SocketState=TERMINATE_SERVER;
            return;
        }

        if( errno==ENOPKG )
        {
            LogMessage( LOG_EMERG, "ConnectThread::InitReceiveSocket", "ENOPKG error :  A kernel subsystem was not configured; Awaiting O/S and Console Intervention" );
            SocketState=Connect_Thread_SUSPEND_FOR_CONSOLE_INTERVENTION;
            return;
        }

        if( errno==ENOPROTOOPT  )
        {
            LogMessage( LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "ENOPROTOOPT error :  Invalid socket option passed; Terminating Server" );
            SocketState=TERMINATE_SERVER;
            return;
        }

        if( errno==EOPNOTSUPP  )
        {
            LogMessage( LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EOPNOTSUPP error :  Invalid socket option passed; Terminating Server" );
            SocketState=TERMINATE_SERVER;
            return;
        }

        if( errno==EPERM )
        {
            LogMessage( LOG_EMERG, "ConnectThread::InitReceiveSocket", "EPERM error : User doesn't have permission to set high priority, change configuration, or send signals to the requested process or group; Awaiting O/S and Console Intervention" );
            SocketState=Connect_Thread_SUSPEND_FOR_CONSOLE_INTERVENTION;
            return;
        }

        if( errno==ESOCKTNOSUPPORT  )
        {
            LogMessage( LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "ESOCKTNOSUPPORT error :   The socket is not configured or an unknown socket type was requested; Terminating Server" );
            SocketState=TERMINATE_SERVER;
            return;
        }

        /* IPV6 level errors */
        if( errno==ENODEV  )
        {
            LogMessage( LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "ENODEV error : The user tried to bind(2) to a link-local IPv6 address, but the sin6_scope_id in the supplied sockaddr_in6 structure is not a valid interface index; Terminating Server" );
            SocketState=TERMINATE_SERVER;
            return;
        }

        LogMessage( LOG_ERR, "ConnectThread::InitReceiveSocket", "socket initialization failed but the errno value was not recognized; Will retry." );
        std::this_thread::sleep_for(std::chrono::milliseconds(InitReceiveSocketRetryDelay));
        ResetSocket();
        SocketState = Connect_Thread_ADDRINFO_INIT;
    }
    else
    {
        SocketState = Connect_Thread_SOCKET_CONFIG;
    }
}

void ConnectThread::SetInitReceiveSocketRetryDelay( int Value )
{
    InitReceiveSocketRetryDelay = Value;
}

void ConnectThread::SetConsoleInterventionPollingDelay( int Value )
{
    ConsoleInterventionPollingDelay = Value;
}




void ConnectThread::InitSocketConfig()
{
//fprintf( stderr, "ConnectThread::InitSocketConfig\n" );
    int    on = 1;
 
    status = ioctl(SocketDescriptor, FIONBIO, (char *)&on);
    if (status < 0)
    {
//fprintf( stderr, "ConnectThread::InitSocketConfig failed to set FIONBIO\n" );
        shutdown( SocketDescriptor, SHUT_RDWR);
        SocketDescriptor = -1;
        SocketState = Connect_Thread_ADDRINFO_INIT;
        return;
    }

    //status = ioctl(SocketDescriptor, TCP_NODELAY, (char *)&on);
    status = setsockopt(SocketDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(int)); 
    if (status < 0)
    {
//fprintf( stderr, "ConnectThread::InitSocketConfig failed to set TCP_NODELAY\n" );
        shutdown( SocketDescriptor, SHUT_RDWR);
        SocketDescriptor = -1;
        SocketState = Connect_Thread_ADDRINFO_INIT;
        return;
    }

    SocketState = Connect_Thread_PORT_INIT;
}

void ConnectThread::InitPort()
{
//fprintf( stderr, "ConnectThread::InitPort\n" );
    // make sure the port is not in use from previous execution of our code.
    int yes = 1;
    status = setsockopt(SocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1)
    {
        printf("setsockopt error\n");
        ResetSocket();
    }
    else
    {
        SocketState = Connect_Thread_BIND_INIT;
    }
}

void ConnectThread::BindSocket()
{
//fprintf( stderr, "ConnectThread::BindSocket\n" );
    status = bind(SocketDescriptor, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)
    {
        printf("bind error\n");
        ResetSocket();
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
        struct sockaddr_in        their_addr;
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
            if( NextPluginModule!=NULL )
            {
                char str[INET_ADDRSTRLEN];

                inet_ntop(AF_INET, &(their_addr.sin_addr), str, INET_ADDRSTRLEN);

//fprintf(stderr, "ConnectThread::ReceiveConnectionAttempts getting empty SocketEntity\n" );
                SocketEntity *NewSocketEntity =  (SocketEntity *)(DataInputPool->Pop_front());
//fprintf(stderr, "ConnectThread::ReceiveConnectionAttempts got empty SocketEntity ok\n" );

                NewSocketEntity->StartupSocket( new_sd, str );

//fprintf(stderr, "ConnectThread::ReceiveConnectionAttempts sending newly populated SocketEntity\n" );
                DataOutputPool->Push_back( (DataModule *)NewSocketEntity );
//fprintf(stderr,"ConnectThread::ReceiveConnectionAttempts Connection accepted. Using new SocketDescriptor : %i\n", NewSocketEntity->GetDescriptor());
            }
        }
    }
}

void ConnectThread::SuspendForConsoleIntervention()
{
    //This is really just a placeholder preventing too-fast-polling and allowing the SocketState
    //to be set by console command once the problem is resolved.
    std::this_thread::sleep_for(std::chrono::milliseconds(ConsoleInterventionPollingDelay));
}

void ConnectThread::TerminateServer()
{
    //this is a stub.
}

void ConnectThread::ConnectionListener()
{
//fprintf( stderr, "ConnectThread::ConnectionListener\n");

    while(SocketState!=Connect_Thread_TERMINATE)
    {
        if( SocketState == Connect_Thread_LISTENING )
        {
            ReceiveConnectionAttempts();
        }
        else if( SocketState == Connect_Thread_HOSTINFO_INIT )
        {
            InitHostInfo();
        }
        else if( SocketState ==  Connect_Thread_ADDRINFO_INIT )
        {
            InitAddressInfo( NULL );
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
        else if( SocketState == Connect_Thread_SUSPEND_FOR_CONSOLE_INTERVENTION )
        {
            SuspendForConsoleIntervention();
        }
        else if( SocketState == TERMINATE_SERVER )
        {
             TerminateServer();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(ConnectionPollingDelay));
    }
}

ConnectThread::ConnectThread()
{
}

ConnectThread::~ConnectThread()
{
}

void ConnectThread::Start()
{
//fprintf( stderr, "ConnectThread::Start begins\n");
    SocketState = Connect_Thread_HOSTINFO_INIT;
    ConnectionListenerThread = std::thread([=] { ConnectionListener(); });
//fprintf( stderr, "ConnectThread::Start ends\n");
}

void ConnectThread::Stop()
{
//fprintf( stderr, "ConnectThread::Stop begins\n");

    SocketState = Connect_Thread_TERMINATE;

    if( host_info_list!=NULL )
    {
        freeaddrinfo(host_info_list);
    }
    ConnectionListenerThread.join();
//fprintf( stderr, "ConnectThread::Stop ends\n");
}

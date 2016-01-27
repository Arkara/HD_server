//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon


#include <iostream>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>





#include "ConnectThread.h"





std::vector<struct ErrorHandlerData> ErrorData = {
    // SOCKET level errors 
    //  errno,   LogLevel,  Class::Method(string),              "Error Message(string)", 
    { EACCES, LOG_EMERG, "ConnectThread::InitReceiveSocket", "EACCESS error: Permission to create listener socket is denied; Awaiting O/S and Console Intervention.", CONNECT_THREAD_SUSPEND_FOR_CONSOLE_INTERVENTION },
    { EAFNOSUPPORT , LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EAFNOSUPPORT error : The implementation does not support the specified address family; Terminating Server.", CONNECT_THREAD_TERMINATE_SERVER },
    { EINVAL, LOG_EMERG, "ConnectThread::InitReceiveSocket", "EINVAL error : Unknown protocol, or protocol family not available; Awaiting O/S and Console Intervention", CONNECT_THREAD_SUSPEND_FOR_CONSOLE_INTERVENTION },
    { EMFILE, LOG_ERR, "ConnectThread::InitReceiveSocket", "EMFILE error : The per-process limit on the number of open file descriptors has been reached; Will Retry.",  CONNECT_THREAD_SOCKET_INIT },
    { ENFILE, LOG_ERR, "ConnectThread::InitReceiveSocket", "ENFILE error : The system-wide limit on the total number of open files has been reached; Will Retry.",  CONNECT_THREAD_SOCKET_INIT },
    { ENOBUFS, LOG_ERR, "ConnectThread::InitReceiveSocket", "ENOBUFS error : The socket cannot be created until sufficient resources are freed; Will Retry",  CONNECT_THREAD_SOCKET_INIT },
    { ENOMEM, LOG_ERR, "ConnectThread::InitReceiveSocket", "ENOMEM error : The socket cannot be created until sufficient resources are freed; Will Retry",  CONNECT_THREAD_SOCKET_INIT },
    { EPROTONOSUPPORT, LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EPROTONOSUPPORT error : The protocol type or the specified protocol is not supported within this domain; Terminating Server", CONNECT_THREAD_TERMINATE_SERVER },
// IPV4 level errors 
    { EADDRINUSE, LOG_EMERG, "ConnectThread::InitReceiveSocket", "EADDRINUSE error :  Tried to bind to an address already in use; Awaiting O/S and Console Intervention", CONNECT_THREAD_SUSPEND_FOR_CONSOLE_INTERVENTION },
    { EADDRNOTAVAIL, LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EADDRNOTAVAIL error :  A nonexistent interface was requested or the requested source address was not local; Terminating Server", CONNECT_THREAD_TERMINATE_SERVER },
    { ENOPKG, LOG_EMERG, "ConnectThread::InitReceiveSocket", "ENOPKG error :  A kernel subsystem was not configured; Awaiting O/S and Console Intervention", CONNECT_THREAD_SUSPEND_FOR_CONSOLE_INTERVENTION },
    { ENOPROTOOPT, LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "ENOPROTOOPT error :  Invalid socket option passed; Terminating Server", CONNECT_THREAD_TERMINATE_SERVER },
    { EOPNOTSUPP, LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "EOPNOTSUPP error :  Invalid socket option passed; Terminating Server", CONNECT_THREAD_TERMINATE_SERVER },
    { EPERM, LOG_EMERG, "ConnectThread::InitReceiveSocket", "EPERM error : User doesn't have permission to set high priority, change configuration, or send signals to the requested process or group; Awaiting O/S and Console Intervention", CONNECT_THREAD_SUSPEND_FOR_CONSOLE_INTERVENTION },
    { ESOCKTNOSUPPORT, LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "ESOCKTNOSUPPORT error :   The socket is not configured or an unknown socket type was requested; Terminating Server", CONNECT_THREAD_TERMINATE_SERVER },
// IPV6 level errors 
    { ENODEV , LOG_TERMINAL, "ConnectThread::InitReceiveSocket", "ENODEV error : The user tried to bind(2) to a link-local IPv6 address, but the sin6_scope_id in the supplied sockaddr_in6 structure is not a valid interface index; Terminating Server", CONNECT_THREAD_TERMINATE_SERVER } };





void ConnectThread::ResetSocket()
{
    shutdown( SocketDescriptor, SHUT_RDWR);
    SocketDescriptor = -1;
    SocketState = CONNECT_THREAD_ADDRINFO_INIT;
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
    SocketState = CONNECT_THREAD_ADDRINFO_INIT;
}

void ConnectThread::InitAddressInfo(const char *TargetHostURL)
{
//fprintf( stderr, "ConnectThread::InitHostInfo\n" );
    status = getaddrinfo(TargetHostURL, "56124", &host_info, &host_info_list);
    if (status != 0)
    {
        printf("getaddrinfo error %s\n", gai_strerror(status));
        SocketState = CONNECT_THREAD_HOSTINFO_INIT;
    }
    else
    {
        SocketState = CONNECT_THREAD_SOCKET_INIT;
    }
}

void ConnectThread::InitReceiveSocket()
{
//fprintf( stderr, "ConnectThread::InitReceiveSocket\n" );
    SocketDescriptor = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (SocketDescriptor == -1)
    {
        ConnectThread::HandleErrorOnReceiveSocket( errno);
    }
    else
    {
        SocketState = CONNECT_THREAD_SOCKET_CONFIG;
    }
}

void ConnectThread::HandleErrorOnReceiveSocket(int ErrorNo )
{
    
    for( int currentEntry=0; currentEntry<ErrorData.size(); currentEntry++ )
    {
        if( ErrorData[ currentEntry ].errnoCode == ErrorNo &&  ErrorData[ currentEntry ].classAndMethod=="ConnectThread::InitReceiveSocket" )
        {
            LogMessage( ErrorData[ currentEntry ].logLevel,
                "ConnectThread::InitReceiveSocket",
                 ErrorData[ currentEntry ].errorMessage.data() );
            SocketState = ErrorData[ currentEntry ].nextState;
            return;
        }
    }


    LogMessage( LOG_ERR, "ConnectThread::InitReceiveSocket", "socket initialization failed but the errno value was not recognized; Will retry." );
    std::this_thread::sleep_for(std::chrono::milliseconds(InitReceiveSocketRetryDelay));
    ResetSocket();
    SocketState = CONNECT_THREAD_ADDRINFO_INIT;
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
        SocketState = CONNECT_THREAD_ADDRINFO_INIT;
        return;
    }

    //status = ioctl(SocketDescriptor, TCP_NODELAY, (char *)&on);
    status = setsockopt(SocketDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(int)); 
    if (status < 0)
    {
//fprintf( stderr, "ConnectThread::InitSocketConfig failed to set TCP_NODELAY\n" );
        shutdown( SocketDescriptor, SHUT_RDWR);
        SocketDescriptor = -1;
        SocketState = CONNECT_THREAD_ADDRINFO_INIT;
        return;
    }

    SocketState = CONNECT_THREAD_PORT_INIT;
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
        SocketState = CONNECT_THREAD_BIND_INIT;
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
        SocketState = CONNECT_THREAD_LISTENING;
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

    while(SocketState!=CONNECT_THREAD_TERMINATE)
    {
        if( SocketState == CONNECT_THREAD_LISTENING )
        {
            ReceiveConnectionAttempts();
        }
        else if( SocketState == CONNECT_THREAD_HOSTINFO_INIT )
        {
            InitHostInfo();
        }
        else if( SocketState ==  CONNECT_THREAD_ADDRINFO_INIT )
        {
            InitAddressInfo( NULL );
        }
        else if( SocketState == CONNECT_THREAD_SOCKET_INIT )
        {
            InitReceiveSocket();
        }
        else if( SocketState == CONNECT_THREAD_SOCKET_CONFIG )
        {
            InitSocketConfig();
        }
        else if( SocketState == CONNECT_THREAD_PORT_INIT )
        {
            InitPort();
        }
        else if( SocketState == CONNECT_THREAD_BIND_INIT )
        {
            BindSocket();
        }
        else if( SocketState == CONNECT_THREAD_SUSPEND_FOR_CONSOLE_INTERVENTION )
        {
            SuspendForConsoleIntervention();
        }
        else if( SocketState == CONNECT_THREAD_TERMINATE_SERVER )
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
    SocketState = CONNECT_THREAD_HOSTINFO_INIT;
    ConnectionListenerThread = std::thread([=] { ConnectionListener(); });
//fprintf( stderr, "ConnectThread::Start ends\n");
}

void ConnectThread::Stop()
{
//fprintf( stderr, "ConnectThread::Stop begins\n");

    SocketState = CONNECT_THREAD_TERMINATE;

    if( host_info_list!=NULL )
    {
        freeaddrinfo(host_info_list);
    }
    ConnectionListenerThread.join();
//fprintf( stderr, "ConnectThread::Stop ends\n");
}

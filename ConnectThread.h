//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon

#ifndef _HD_ConnectThread

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <atomic>




#include "FlowControlModules.h"
#include "SocketThreadLeader.h"






#define CONNECT_THREAD_TERMINATE                          0
#define CONNECT_THREAD_HOSTINFO_INIT                      1
#define CONNECT_THREAD_ADDRINFO_INIT                      2
#define CONNECT_THREAD_SOCKET_INIT                        3
#define CONNECT_THREAD_PORT_INIT                          4
#define CONNECT_THREAD_BIND_INIT                          5
#define CONNECT_THREAD_LISTENING                          6
#define CONNECT_THREAD_SOCKET_CONFIG                      7
#define CONNECT_THREAD_SUSPEND_FOR_CONSOLE_INTERVENTION   8
#define CONNECT_THREAD_TERMINATE_SERVER                   9

class ConnectThread : public PluginModule
{
protected :
    std::thread              ConnectionListenerThread;
    std::atomic<int>         SocketState;
    int                      SocketDescriptor;

    void ResetSocket();

private :
    struct addrinfo          host_info;
    struct addrinfo          *host_info_list = NULL;
    int                      status;
    int                      ConnectionPollingDelay = 100;

    int                      InitReceiveSocketRetryDelay = 10000;
    int                      ConsoleInterventionPollingDelay = 10000; 

    void InitHostInfo();

    void InitReceiveSocket();

    void InitAddressInfo(const char *TargetHostURL);
    void InitSocketConfig();
    void InitPort();
    void BindSocket();
    void ReceiveConnectionAttempts();
    void SuspendForConsoleIntervention();
    void TerminateServer();

public :
    ConnectThread();
    ~ConnectThread();

    void Start();
    void Stop();

    void ConnectionListener();

    void SetInitReceiveSocketRetryDelay( int Value );
    void SetConsoleInterventionPollingDelay( int Value );
    void HandleErrorOnReceiveSocket( int ErrorNo);

};

struct ErrorHandlerData {
    int    errnoCode;
    int    logLevel;
    std::string   classAndMethod;
    std::string   errorMessage;
    int    nextState;
};



#define _HD_ConnectThread
#endif




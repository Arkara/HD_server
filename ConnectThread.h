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






#define Connect_Thread_TERMINATE                          0
#define Connect_Thread_HOSTINFO_INIT                      1
#define Connect_Thread_ADDRINFO_INIT                      2
#define Connect_Thread_SOCKET_INIT                        3
#define Connect_Thread_PORT_INIT                          4
#define Connect_Thread_BIND_INIT                          5
#define Connect_Thread_LISTENING                          6
#define Connect_Thread_SOCKET_CONFIG                      7
#define Connect_Thread_SUSPEND_FOR_CONSOLE_INTERVENTION   8
#define TERMINATE_SERVER                                  9

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

};


#define _HD_ConnectThread
#endif




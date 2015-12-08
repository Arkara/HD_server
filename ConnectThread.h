#ifndef _HD_ConnectThread

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <atomic>




#include "FlowControlModules.h"
#include "SocketThreadLeader.h"






#define Connect_Thread_TERMINATE          0
#define Connect_Thread_GETADDRINFO_INIT   1
#define Connect_Thread_SOCKET_INIT        2
#define Connect_Thread_PORT_INIT          3
#define Connect_Thread_BIND_INIT          4
#define Connect_Thread_LISTENING          5
#define Connect_Thread_SOCKET_CONFIG      6

class ConnectThread : public PluginModule
{
private :
    std::thread              ConnectionListenerThread;
    int                      status;
    struct addrinfo          host_info;
    struct addrinfo          *host_info_list = NULL;
    int                      SocketDescriptor;
    int                      ConnectionPollingDelay = 100;

    void InitHostInfo(const char *TargetHostURL);
    void InitReceiveSocket();
    void InitSocketConfig();
    void InitPort();
    void BindSocket();
    void ReceiveConnectionAttempts();
 
    std::atomic<int> SocketState;

public :
   
    ConnectThread();
    ~ConnectThread();
    void ConnectionListener();

//FlowControl operations : ConnectThread only gets SocketEntitys from the next layer
//    and passes them back so doesn't need to define any of the methods for itself,
//    only to know how to call them.
};


#define _HD_ConnectThread
#endif




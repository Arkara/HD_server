#ifndef _HD_ConnectThread

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <mutex>




#include "SocketThread.h"





void ConnectionListener(int tid);

class ConnectThread
{
private :
    std::thread ConnectionListenerThreads;
    SocketThreads* SocketHandlerThreads;

public :
    ConnectThread( SocketThreads* SocketHandlerThreads );
    ~ConnectThread();
};


#define _HD_ConnectThread
#endif

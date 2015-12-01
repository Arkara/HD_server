#pragma once

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <mutex>


//ConnectionListenerThread
//listen on port for connections
//for each connection
// create socket
// pass to thread with least sockets
void ConnectionListener(int tid);

class ConnectThread
{
private :
    std::thread ConnectionListenerThreads;

public :
    ConnectThread();
    ~ConnectThread();
};




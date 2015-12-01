#include "ConnectThread.h"
#include <iostream>




//ConnectionListenerThread
//listen on port for connections
//for each connection
// create socket
// pass to thread with least sockets
void ConnectionListener(int tid) {
    std::cout << "XXXX" << std::endl;
}

ConnectThread::ConnectThread()
{
     ConnectionListenerThreads = std::thread( ConnectionListener, 0 );
}

ConnectThread::~ConnectThread()
{
     ConnectionListenerThreads.join();
}

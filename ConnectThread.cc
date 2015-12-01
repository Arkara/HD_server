#include "ConnectThread.h"
#include <iostream>




void ConnectionListener(int tid) {
    std::cout << "XXXX" << std::endl;
}

ConnectThread::ConnectThread(  SocketThreads* pSocketHandlerThreads )
{
     SocketHandlerThreads=pSocketHandlerThreads;
     ConnectionListenerThreads = std::thread(  ConnectionListener, 0 );
}

ConnectThread::~ConnectThread()
{
     ConnectionListenerThreads.join();
}

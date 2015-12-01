#pragma once

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>


//SocketHandlerThread
//for each entry in list of sockets 
// check for input or activity 
// process input or activity
void SocketHandlerThread(int tid);

class SocketThreads
{
private :
    std::list<std::thread> SocketHandlerThreads;
    int NumberOfSocketHandlerThreads = 1;
    int NumberOfCores = 1;

public :
    SocketThreads();
    ~SocketThreads();
};


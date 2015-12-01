#include "SocketThread.h"
#include "Utility.h"
#include <iostream>
#include <thread>




//SocketHandlerThread
//for each entry in list of sockets 
// check for input or activity 
// process input or activity
void SocketHandlerThread(int tid) {
    std::cout << tid << std::endl;
sleep(5);
}

SocketThreads::SocketThreads()
{
    NumberOfSocketHandlerThreads = 1;
    NumberOfCores = GetNumberOfCPUCores();

    if( NumberOfCores>1 )
    {
        NumberOfSocketHandlerThreads=NumberOfCores-1;
    }

    for( int i=0; i<NumberOfSocketHandlerThreads; i++ )
    {
        SocketHandlerThreads.push_back( std::thread(  SocketHandlerThread, i ) );
    }
}

SocketThreads::~SocketThreads()
{
    for (std::list<std::thread>::iterator SocketHandlerThreadIterator=SocketHandlerThreads.begin(); SocketHandlerThreadIterator!=SocketHandlerThreads.end() ; ++SocketHandlerThreadIterator)
    {
        SocketHandlerThreadIterator->join();
    }
}







// standard libraries
#include <iostream>
#include <stdio.h>





//Harikan Dawn custom libraries
#include "SocketThread.h"
#include "ConnectThread.h"
#include "Utility.h"





int main()
{
    bool   EndProgram = false;
    char   InputBuffer[1024];


    printf( "\nSystem Information:\nNumber of Cores : %i\nNumber of Threads Supported : %u\n\n\n",
        GetNumberOfCPUCores(),
        GetNumberOfThreadsSupported()
    );

    SocketThreads SocketHandlerThreads;
    ConnectThread ConnectListenerThread;

    while ( !EndProgram )
    {
        scanf( "%1023s", InputBuffer );
        if( strstr(InputBuffer,"quit")==InputBuffer )
        {
            EndProgram = true;
        }
    }
}


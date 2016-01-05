//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon

// standard libraries
#include <iostream>
#include <stdio.h>





//Harikan Dawn custom libraries
#include "FlowControlModules.h"
#include "SocketThreadLeader.h"
#include "ConnectThread.h"
#include "Utility.h"

//SocketThreadLeader SocketLeader;
//ConnectThread ConnectListenerThread;
//DataModulePool SocketPool;

void ApplyConfiguration()
{
    //eventually override with values read from a config file 
}

int main()
{
    bool   EndProgram = false;
    char   InputBuffer[1024];
    SocketEntity SocketProvider( -1, (char *)"0.0.0.0" );

    DataModulePool UnusedSocketPool;
    UnusedSocketPool.SetProvider( &SocketProvider );

    DataModulePool SocketPool;
    SocketPool.SetProvider( &UnusedSocketPool );  //note if called on for a socket and none is waiting, provide an enpty to avoid returning a null, which is just a bad practice.

    ConnectThread ConnectListenerThread;
    ConnectListenerThread.SetInputPool( &UnusedSocketPool );
    ConnectListenerThread.SetOutputPool( &SocketPool );
    ConnectListenerThread.Start();

    SocketThreadLeader SocketLeader;
    SocketLeader.SetInputPool( &SocketPool );
    SocketLeader.SetOutputPool( &UnusedSocketPool );

    ConnectListenerThread.SetNextPluginModule( (PluginModule *)&SocketLeader );
    


printf( "Harikan Dawn Server rev 0.0.1\n" );

    ApplyConfiguration();

    while ( !EndProgram )
    {
        scanf( "%1023s", InputBuffer );
        if( strstr(InputBuffer,"quit")==InputBuffer )
        {
            EndProgram = true;
            ConnectListenerThread.Stop();
        }
    }
}


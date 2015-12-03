// standard libraries
#include <iostream>
#include <stdio.h>





//Harikan Dawn custom libraries
#include "SocketThreadLeader.h"
#include "ConnectThread.h"
#include "Utility.h"


SocketThreadLeader SocketLeader;
ConnectThread ConnectListenerThread;

void ApplyConfiguration()
{
    //eventually override with values read from a config file 
}

int main()
{
    bool   EndProgram = false;
    char   InputBuffer[1024];


printf( "Harikan Dawn Server rev 0.0.1\n" );

    ApplyConfiguration();

    while ( !EndProgram )
    {
        printf ( " HD: " );
        scanf( "%1023s", InputBuffer );
        if( strstr(InputBuffer,"quit")==InputBuffer )
        {
            EndProgram = true;
        }
    }
}


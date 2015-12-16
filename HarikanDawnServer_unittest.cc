//GoogleTest API is Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------
// Tests and application code called from them are
// Copyright (C) 2015, Harikan Dawn Associates.All Rights Reserved.
// Contributors include : Joe A Lyon.
//



// standard libraries
#include <iostream>
#include <stdio.h>





//Harikan Dawn custom libraries
#include "FlowControlModules.h"
#include "SocketThreadLeader.h"
#include "ConnectThread.h"
#include "Utility.h"


#include "gtest/gtest.h"


void ApplyConfiguration()
{
    //eventually override with values read from a config file 
}

int main(int ac, char* av[])
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

    SocketThreadLeader SocketLeader;
    SocketLeader.SetInputPool( &SocketPool );
    SocketLeader.SetOutputPool( &UnusedSocketPool );

    ConnectListenerThread.SetNextPluginModule( (PluginModule *)&SocketLeader );
    


//printf( "Harikan Dawn Server rev 0.0.1\n" );

//    ApplyConfiguration();

//    while ( !EndProgram )
//    {
//        scanf( "%1023s", InputBuffer );
//        if( strstr(InputBuffer,"quit")==InputBuffer )
//        {
//            EndProgram = true;
//        }
//    }

  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();

}


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


// Testing framework libraries
#include "gtest/gtest.h"





// standard libraries
#include <iostream>
#include <stdio.h>





//Harikan Dawn custom libraries
#include "ConnectThread.h"
#include "FlowControlModules.h"

class TestableConnectThread : public ConnectThread
{

public :
    void resetSocket()
    {
        ResetSocket();
    }

    std::thread *GetThread()
    {
        return &ConnectionListenerThread;
    }
    int GetSocketState()
    {
        return SocketState;
    }

    int SetSocketState(int StateValue )
    {
        SocketState = StateValue;
    }

    int GetSocketDescriptor()
    {
        return SocketDescriptor;
    }
    int SetSocketDescriptor(int DescriptorValue)
    {
        SocketDescriptor = DescriptorValue;
    }
};

TestableConnectThread ObjectUnderTest;

TEST( testfacility, proof )
{
    EXPECT_TRUE( true );
}

TEST( Threading, Start )
{
    ObjectUnderTest.Start();
    EXPECT_TRUE( ObjectUnderTest.GetThread()->joinable() );
    ObjectUnderTest.Stop();
}

TEST( Threading, Status_HostInfoInit )
{
    ObjectUnderTest.Start();
    EXPECT_EQ( Connect_Thread_HOSTINFO_INIT, ObjectUnderTest.GetSocketState() );
    ObjectUnderTest.Stop();
}

TEST( Threading, Stop )
{
    ObjectUnderTest.Start();
    ObjectUnderTest.Stop();
    EXPECT_FALSE( ObjectUnderTest.GetThread()->joinable() );
}

TEST( Threading, Status_Terminate )
{
    ObjectUnderTest.Start();
    ObjectUnderTest.Stop();
    EXPECT_EQ( Connect_Thread_TERMINATE, ObjectUnderTest.GetSocketState() );
}



TEST( Initialization, ResetSocket )
{
    ObjectUnderTest.SetSocketState( 999 );
    ObjectUnderTest.SetSocketDescriptor( 999 );
    ObjectUnderTest.resetSocket();
    EXPECT_EQ( -1, ObjectUnderTest.GetSocketDescriptor() );
    EXPECT_EQ( Connect_Thread_ADDRINFO_INIT, ObjectUnderTest.GetSocketState() );
}






int main(int ac, char* av[])
{

  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();

}


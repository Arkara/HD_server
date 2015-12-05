#include "SocketEntity.h"
#include <thread>
#include "Utility.h"
#include <iostream>




SocketEntity::SocketEntity(int pDescriptor)
{
    StartupSocket( pDescriptor );
}

void SocketEntity::StartupSocket(int pDescriptor)
{
    long CurrentTime =  GetTimeInMilliseconds();

    timestampOfLastHeartbeatReceived = CurrentTime;
    timestampOfLastDataInput = CurrentTime;

    Descriptor = pDescriptor;
    Disconnect = false;
 
}


SocketEntity::~SocketEntity()
{
    ShutdownSocket();
}

void SocketEntity::ShutdownSocket()
{
fprintf(stderr, "SocketEntity::ShutdownSocket ( %i )\n", Descriptor );
    if( Descriptor>0 )
    {
        shutdown( Descriptor, SHUT_RDWR);
        Descriptor = -1;
    }
fprintf(stderr, "SocketEntity::ShutdownSocket ends\n" );
}

int SocketEntity::GetDescriptor()
{
    return Descriptor;
}

void SocketEntity::ServiceSocket()
{
//fprintf( stderr, "SocketEntity::ServiceSocket %i begins\n", Descriptor );
    CheckForData();

    long MostRecentOutput;
    long MostRecentInput;
    long CurrentTime =  GetTimeInMilliseconds();


    MostRecentOutput=timestampOfLastHeartbeatSent>timestampOfLastDataOutput?timestampOfLastHeartbeatSent:timestampOfLastDataOutput;
    if( MostRecentOutput+OutgoingQuietMilliseconds < CurrentTime )
    {
        SendHeartbeat();
    }

    MostRecentInput=timestampOfLastHeartbeatReceived>timestampOfLastDataInput?timestampOfLastHeartbeatReceived:timestampOfLastDataInput;
    if( MostRecentInput+DisconnectMilliseconds < CurrentTime )
    {
        MarkForDisconnect();
    }
//fprintf( stderr, "SocketEntity::ServiceSocket %i ends\n", Descriptor );
}

/////
// 
//  Here we only know about sending and receiving data.
//  IF We are going to emulate SCTP it would happen at a layer obove this.
//  Encryption and compression would also happen at a higher level.
//
//  The one thing this level does need to understand is if we are doing TCP
//    or UDP. If UDP, we'll need to implement buffering, sequence tracking, 
//    retransmission and lost packet requesting...which begs the question
//    of why aren't we using the low level facilities for those things that
//    TCP provides and then tuning it to meet our needs?
//    I hope TCP works OK for us, though I'm told it won't.
//
void SocketEntity::CheckForData()
{
//fprintf( stderr, "SocketEntity::CheckForData ( %i )\n", Descriptor );
    while( true )
    {
        char buffer[1024];
        memset( buffer, 0, 1024 );

        int rc = recv( Descriptor, buffer, sizeof(buffer), MSG_DONTWAIT);

        if (rc < 0)
        {
            if (errno != EWOULDBLOCK && errno != EAGAIN )
            {
                fprintf( stderr, "SocketEntity::CheckForData ( %i ) got error %i\n", Descriptor, errno );
                MarkForDisconnect();
            }
            break;
        }
        else if ( rc==0 )
        {
            fprintf( stderr, "SocketEntity::CheckForData client closed connection\n" );
            MarkForDisconnect();
            break;
        }
        else
        {
            if( buffer[0]==1 && buffer[1]==1 && buffer[2]==0 )
            {
                timestampOfLastHeartbeatReceived = GetTimeInMilliseconds();
            }
            else
            {
                timestampOfLastDataInput = GetTimeInMilliseconds();
                fprintf( stderr, "%i )  %s\n", Descriptor, buffer );
            }
        }
    }
}

char HeartbeatMessage[] = { 1, 1, 0 };

void SocketEntity::SendHeartbeat()
{
fprintf( stderr, "SocketEntity::SendHeartbeat\n" );

    timestampOfLastHeartbeatSent=GetTimeInMilliseconds();

    SendWrapper( HeartbeatMessage, strlen(HeartbeatMessage) );
    
}

void SocketEntity::SendData( char *bdata, long dataLength )
{
fprintf( stderr, "SocketEntity::SendData\n" );
            
    SendWrapper( bdata, dataLength );
    timestampOfLastDataOutput=GetTimeInMilliseconds();
}

void SocketEntity::SendWrapper( char *bdata, long dataLength )
{
fprintf( stderr, "SocketEntity::SendWrapper\n" );
    long retValue;
    char buffer[1024];

    memset( buffer, 0, 1024 );
    memcpy( buffer, bdata, dataLength>1023?1023:dataLength );
    buffer[1023]=0;

    retValue = send( Descriptor, buffer, dataLength, MSG_NOSIGNAL|MSG_DONTWAIT );

    if( retValue <0 )
    {
        if( errno!=EAGAIN && errno!=EWOULDBLOCK )
        {
            if( errno==ECONNRESET||errno==EPIPE)
            {
                fprintf( stderr, "SocketEntity::SendWrapper reports connection reset by client\n" );
            }
            else
            {
                fprintf( stderr, "SocketEntity::SendWrapper got error %i and is disconnecting socket\n", errno );
            }
            MarkForDisconnect();
        }
    }
    else if ( retValue != dataLength )
    {
std::cout << "SocketEntity::SendWrapper sent " << retValue << " bytes, expected to send " <<dataLength <<std::endl;
        //fprintf( stderr, "SocketEntity::SendWrapper data size sent does not match expected.\n" );
    }
}


//
////

void SocketEntity::MarkForDisconnect()
{
fprintf( stderr, "SocketEntity::MarkForDisconnect\n" );
    Disconnect = true;
}

bool SocketEntity::IsRequestingDisconnect()
{
    bool retDisconnect = Disconnect;

    return retDisconnect;
}

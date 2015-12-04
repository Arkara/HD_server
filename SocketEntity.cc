#include "SocketEntity.h"
#include <thread>
//#include <chrono>
#include "Utility.h"




SocketEntity::SocketEntity(int pDescriptor)
{
    Descriptor = pDescriptor;
}

SocketEntity::~SocketEntity()
{
}

void SocketEntity::ShutdownSocket()
{
fprintf(stderr, "SocketEntity::ShutdownSocket ( %i )\n", Descriptor );
    shutdown( Descriptor, SHUT_RDWR);
fprintf(stderr, "SocketEntity::ShutdownSocket ends\n" );
}

int SocketEntity::GetDescriptor()
{
    return Descriptor;
}

void SocketEntity::ServiceSocket()
{
    CheckForData();
//    DoHeartBeat();
}

void SocketEntity::CheckForData()
{
//fprintf( stderr, "SocketEntity::CheckForData ( %i )\n", Descriptor );
    while( true )
    {
        char buffer[1024];
        memset( buffer, 0, 1024 );

        int rc = recv( Descriptor, buffer, sizeof(buffer), 0);

        if (rc < 1)
        {
            if (errno != EWOULDBLOCK && errno != EAGAIN )
            {
//fprintf( stderr, "SocketEntity::CheckForData ( %i ) got error %i\n", Descriptor, errno );
            }
            break;
        }
        else
        {
            if( buffer[0]=='H' && buffer[1]=='B' && buffer[2]==0 )
            {
                timestampOfLastHeartbeat = GetTimeInMilliseconds();
            }
            else
            {
                timestampOfLastDataInput = GetTimeInMilliseconds();
                fprintf( stderr, "%i )  %s\n", Descriptor, buffer );
            }
        }
    }
}

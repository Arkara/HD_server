#include "SocketEntity.h"
#include <thread>

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

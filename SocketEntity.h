#ifndef _SOCKET_ENTITY
#define _SOCKET_ENTITY

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>



class SocketEntity
{
private :
   int    Descriptor;
   double timestampOfLastHeartbeat = 0;
   double timestampOfLastDataInput = 0;

   void ServiceSocket();

public :
    SocketEntity(int pDescriptor);
    ~SocketEntity();

    int GetDescriptor();
    void ShutdownSocket();
};

#endif

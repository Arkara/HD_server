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
    long timestampOfLastHeartbeat = 0;
    long timestampOfLastDataInput = 0;

    void CheckForData();
    void DoHeartBeat();
public :
    SocketEntity(int pDescriptor);
    ~SocketEntity();

    int GetDescriptor();
    void ShutdownSocket();
    void ServiceSocket();
};

#endif

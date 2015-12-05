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

    long   timestampOfLastHeartbeatSent = 0;
    long   timestampOfLastDataOutput = 0;
    int    OutgoingQuietMilliseconds = 5000;

    long   timestampOfLastHeartbeatReceived;
    long   timestampOfLastDataInput;
    int    DisconnectMilliseconds = 30000;

    bool Disconnect;

    void CheckForData();
    void SendHeartbeat();
    void SendWrapper(char *bdata, long dataLength);

public :
    SocketEntity(int pDescriptor);
    ~SocketEntity();

    void StartupSocket(int pDescriptor);
    void ShutdownSocket();

    int GetDescriptor();
    void ServiceSocket();
    void MarkForDisconnect();
    bool IsRequestingDisconnect();
    void SendData( char *bdata, long dataLength );
};

#endif

#ifndef _SOCKET_ENTITY
#define _SOCKET_ENTITY

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>





#include "FlowControlModules.h"


class SocketEntity : public DataModule
{
private :
    int    Descriptor;
    char   IPAddressString[46]; //15 for IPv4 or 45 for IPv6, plus 1 for NULL termination.

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
    SocketEntity(int pDescriptor, char *pIPAddress);
    ~SocketEntity();

    void StartupSocket(int pDescriptor, char *pIPAddress );
    void ShutdownSocket();

    int GetDescriptor();
    void ServiceSocket();
    void MarkForDisconnect();
    bool IsRequestingDisconnect();
    void SendData( char *bdata, long dataLength );

     DataModule *ProvideData();
};

#endif

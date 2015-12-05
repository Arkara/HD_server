#ifndef _SOCKET_THREAD_LEADER
#define _SOCKET_THREAD_LEADER

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>





#include "SocketThreadList.h"
class SocketThreadList;

#define SocketThreadLeader_TERMINATE 0
#define SocketThreadLeader_RUN 1


class SocketThreadLeader
{
private :
    std::thread                  LeaderThread;



    int                          NumberOfSocketLists = 1;
    void CheckCollectionSize();
    void GrowListCollection();
    void ShrinkListCollection();

    std::list<SocketThreadList>  SocketThreadCollection;
    int                          CollectionServicingDelay = 100;
    void ServiceCollection();
    void PointToSmallestList();



    std::atomic_int              State;


    std::list<SocketEntity*>      SocketQueue;
    std::list<SocketEntity*>      UnusedSocketQueue;


    int                          MaxToPullFromQueue = 10;


public :
    SocketThreadLeader();
    ~SocketThreadLeader();

    void SetNumberOfSocketLists(int pNumberOfSocketLists);

    void AddSocketEntityToQueue(SocketEntity *pSocket);
    void AddSocketEntityToUnusedQueue(SocketEntity *pSocket);

    SocketEntity *GetUnusedSocketEntity();
    SocketEntity *GetHeadOfQueue();

    int GetMaxToPullFromQueue();
    int GetQueueSize();
};


extern class SocketThreadLeader SocketLeader;

#endif


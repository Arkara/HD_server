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


    std::atomic_int              State;


    SocketThreadList             *CurrentSmallestList = NULL;
    void PointToSmallestList();



    std::list<SocketEntity>      SocketQueue;

    int                          MaxToPullFromQueue = 10;


public :
    SocketThreadLeader();
    ~SocketThreadLeader();

    void SetNumberOfSocketLists(int pNumberOfSocketLists);
    SocketThreadList* GetSmallestList();

    void AddSocketEntityToQueue(SocketEntity pSocket);
    int GetMaxToPullFromQueue();
    SocketEntity GetHeadOfQueue();
    int GetQueueSize();
};


extern class SocketThreadLeader SocketLeader;

#endif


#ifndef _SOCKET_THREAD_LIST
#define _SOCKET_THREAD_LIST

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <atomic>




#include "SocketEntity.h"
#include "SocketThreadLeader.h"

#define SocketThreadList_TERMINATE 0
#define SocketThreadList_RUN 1



class SocketThreadList
{
private :
    std::list<SocketEntity>    SocketDescriptors;
    std::thread                WorkerThread; 
    int                        ListPollingDelayInMilliseconds = 100;
    std::atomic_bool           TaggedForDeletion;
    std::atomic_int            State;
    bool                       Smallest = false;

    void ServiceList();
    void FlushToLeaderQueue();
    void RetrieveFromLeaderQueue();
    void PollSockets();

public :
    SocketThreadList();
    ~SocketThreadList();

    void TagForDeletion();
    int GetListSize();
    void Terminate();
    void SetSmallest(bool pValue );
};

#endif

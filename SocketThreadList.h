//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon

#ifndef _SOCKET_THREAD_LIST
#define _SOCKET_THREAD_LIST

#include <thread>
#include <list>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <atomic>




#include "SocketEntity.h"
class SocketThreadLeader;
#include "SocketThreadLeader.h"

#define SocketThreadList_TERMINATE 0
#define SocketThreadList_RUN 1



class SocketThreadList
{
private :
    std::list<SocketEntity*>   SocketDescriptors;
    std::thread                WorkerThread; 
    int                        ListPollingDelayInMilliseconds = 10;
    std::atomic_bool           TaggedForDeletion;
    std::atomic_int            State;
    bool                       Smallest = false;
    unsigned                   ListID;
    SocketThreadLeader         *Leader = NULL;

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

    unsigned GetSocketListID();
    void SetSocketListID(unsigned pListID);

    void SetLeader( SocketThreadLeader *pLeader );
};

#endif

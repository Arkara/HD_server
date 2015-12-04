#include "SocketThreadList.h"
#include <thread>
#include <chrono>





void SocketThreadList::ServiceList()
{
    while( State!=SocketThreadList_TERMINATE )
    {
        if( TaggedForDeletion )
        {
            FlushToLeaderQueue();
            break;
        }
        else
        {
            RetrieveFromLeaderQueue();
            PollSockets();
        }
       std::this_thread::sleep_for(std::chrono::milliseconds(ListPollingDelayInMilliseconds));
    }
}

void SocketThreadList::TagForDeletion()
{
    TaggedForDeletion = true;
}

void SocketThreadList::FlushToLeaderQueue()
{

}

void SocketThreadList::RetrieveFromLeaderQueue()
{
//fprintf(stderr, "SocketThreadList::RetrieveFromLeaderQueue begins\n" );

    if( Smallest && SocketLeader.GetQueueSize()>0 )
    {
        for( int NumberRetrieved=0; NumberRetrieved<SocketLeader.GetMaxToPullFromQueue(); NumberRetrieved++ )
        {
            SocketEntity  CurrentSocketEntity = SocketLeader.GetHeadOfQueue();

            if( CurrentSocketEntity.GetDescriptor()==-1 ) //we reached the end of the list
            {
                CurrentSocketEntity.~SocketEntity();
                break;
            }
            else
            {
fprintf(stderr, "SocketThreadList::RetrieveFromLeaderQueue got SocketDescriptor %i\n", CurrentSocketEntity.GetDescriptor() );
                SocketDescriptors.push_back(CurrentSocketEntity);
            }
        }
    }
//fprintf(stderr, "SocketThreadList::RetrieveFromLeaderQueue ends\n" );
}

void SocketThreadList::PollSockets()
{
    //seems like at some point we should decide upon a size for network packets or allow it to be tunable or something; if tunable we have
    //    to devise a (re-)negotiation protocol for hot changes, etc. for now I'm going to arbitrarily say max data block size of 1024 bytes.
    char    buffer[1024];

    for (std::list<SocketEntity>::iterator SocketEntityIterator=SocketDescriptors.begin(); SocketEntityIterator!=SocketDescriptors.end() ; ++SocketEntityIterator)
    {
         SocketEntityIterator->ServiceSocket();
    }
}

SocketThreadList::SocketThreadList()
{
fprintf(stderr, "SocketThreadList::SocketThreadList begins\n");
    TaggedForDeletion = false;
    State=SocketThreadList_RUN;
    WorkerThread = std::thread([=] { ServiceList(); });
fprintf(stderr, "SocketThreadList::SocketThreadList ends\n");
}


SocketThreadList::~SocketThreadList()
{
fprintf(stderr, "SocketThreadList::~SocketThreadList begins\n");
    Terminate();      
    for (std::list<SocketEntity>::iterator SocketEntityIterator=SocketDescriptors.begin(); SocketEntityIterator!=SocketDescriptors.end() ; ++SocketEntityIterator)
    {
         SocketEntityIterator->ShutdownSocket();
    }
    SocketDescriptors.erase(SocketDescriptors.begin(),SocketDescriptors.end());
     
    WorkerThread.join();
fprintf(stderr, "SocketThreadList::~SocketThreadList ends\n");
}


int SocketThreadList::GetListSize()
{
    return SocketDescriptors.size();
}

void SocketThreadList::Terminate()
{
    State=SocketThreadList_TERMINATE;
}

void SocketThreadList::SetSmallest(bool pValue)
{
    Smallest=pValue;
}

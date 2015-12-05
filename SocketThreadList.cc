#include "SocketThreadList.h"
#include <thread>
#include <chrono>
#include <iostream>






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
            SocketEntity  *CurrentSocketEntity = SocketLeader.GetHeadOfQueue();

            if( CurrentSocketEntity->GetDescriptor()==-1 ) //we reached the end of the list
            {
                SocketLeader.AddSocketEntityToUnusedQueue( CurrentSocketEntity );
                break;
            }
            else
            {
fprintf(stderr, "SocketThreadList( %u )::RetrieveFromLeaderQueue got SocketDescriptor %i\n", ListID, CurrentSocketEntity->GetDescriptor() );
                SocketDescriptors.push_back(CurrentSocketEntity);
            }
        }
    }
//fprintf(stderr, "SocketThreadList::RetrieveFromLeaderQueue ends\n" );
}

void SocketThreadList::PollSockets()
{
    for (std::list<SocketEntity*>::iterator SocketEntityIterator=SocketDescriptors.begin(); SocketEntityIterator!=SocketDescriptors.end() ; /* ++SocketEntityIterator */)
    {
        if( (*SocketEntityIterator)->IsRequestingDisconnect() )
        {
            long mysize=SocketDescriptors.size();
std::cout << "SocketThreadList::PollSockets erasing begins with list size "<< mysize << std::endl;

            std::list<SocketEntity*>::iterator eraseTarget=SocketEntityIterator;
            ++SocketEntityIterator;
            (*eraseTarget)->ShutdownSocket();
            SocketLeader.AddSocketEntityToUnusedQueue( *eraseTarget );
            SocketDescriptors.erase( eraseTarget );


            mysize=SocketDescriptors.size();
std::cout << "SocketThreadList::PollSockets erasing ends with list size "<< mysize << std::endl;
        }
        else
        {
            (*SocketEntityIterator)->ServiceSocket();
            ++SocketEntityIterator;
        }
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
    for (std::list<SocketEntity*>::iterator SocketEntityIterator=SocketDescriptors.begin(); SocketEntityIterator!=SocketDescriptors.end() ; ++SocketEntityIterator)
    {
         (*SocketEntityIterator)->ShutdownSocket();
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

unsigned SocketThreadList::GetSocketListID()
{
    return ListID;
}

void SocketThreadList::SetSocketListID(unsigned pListID)
{
    ListID = pListID;
}

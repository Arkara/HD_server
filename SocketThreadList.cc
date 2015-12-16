#include "SocketThreadList.h"
#include <thread>
#include <chrono>
#include <iostream>






SocketThreadList::SocketThreadList( )
{
//fprintf(stderr, "SocketThreadList::SocketThreadList begins\n");
    TaggedForDeletion = false;
    State=SocketThreadList_RUN;
    WorkerThread = std::thread([=] { ServiceList(); });
//fprintf(stderr, "SocketThreadList::SocketThreadList ends\n");
}

SocketThreadList::~SocketThreadList()
{
//fprintf(stderr, "SocketThreadList::~SocketThreadList begins\n");
    Terminate();      
    for (std::list<SocketEntity*>::iterator SocketEntityIterator=SocketDescriptors.begin(); SocketEntityIterator!=SocketDescriptors.end() ; ++SocketEntityIterator)
    {
        SocketEntity *CurrentSocketEntity =*SocketEntityIterator;
        CurrentSocketEntity->ShutdownSocket();
        delete CurrentSocketEntity;
    }
    SocketDescriptors.erase(SocketDescriptors.begin(),SocketDescriptors.end());
     
    WorkerThread.join();
//fprintf(stderr, "SocketThreadList::~SocketThreadList ends\n");
}



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
    if( Leader!=NULL && Smallest )
    {
        DataModulePool *InputQueue =  Leader->GetInputPool();
        if( InputQueue != NULL )
        {

////fprintf(stderr, "SocketThreadList::RetrieveFromLeaderQueue begins\n" );
            for( int NumberRetrieved=0; NumberRetrieved<Leader->GetMaxToPullFromQueue(); NumberRetrieved++ )
            {
                SocketEntity  *CurrentSocketEntity = (SocketEntity *)(InputQueue->Pop_front());

                if( CurrentSocketEntity== NULL )
                {
                    break;
                }
                else if( CurrentSocketEntity->GetDescriptor()==-1 ) //we reached the end of the list
                {
                    Leader->GetOutputPool()->Push_back( CurrentSocketEntity );
                    break;
                }
                else
                {
//fprintf(stderr, "SocketThreadList( %u )::RetrieveFromLeaderQueue got SocketDescriptor %i\n", ListID, CurrentSocketEntity->GetDescriptor() );
                    SocketDescriptors.push_back(CurrentSocketEntity);
                }
            }
        }
////fprintf(stderr, "SocketThreadList::RetrieveFromLeaderQueue ends\n" );
    }
}

void SocketThreadList::PollSockets()
{
    if( Leader!=NULL )
    {
        for (std::list<SocketEntity*>::iterator SocketEntityIterator=SocketDescriptors.begin(); SocketEntityIterator!=SocketDescriptors.end() ; /* ++SocketEntityIterator */)
        {
            if( (*SocketEntityIterator)->IsRequestingDisconnect() )
            {
                DataModulePool *OutputPool = Leader->GetOutputPool();
                long mysize=SocketDescriptors.size();
std::cout << "SocketThreadList::PollSockets erasing begins with list size "<< mysize << std::endl;

                std::list<SocketEntity*>::iterator eraseTarget=SocketEntityIterator;
                ++SocketEntityIterator;
                (*eraseTarget)->ShutdownSocket();
                if( OutputPool==NULL ) {
                    delete *eraseTarget;
                }
                else
                {
                    OutputPool->Push_back( *eraseTarget );
                }
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

void SocketThreadList::SetLeader( SocketThreadLeader *pLeader )
{
    Leader = pLeader;
}

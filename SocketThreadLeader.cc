#include "SocketThreadLeader.h"
#include "SocketEntity.h"
#include "Utility.h"
#include <iostream>
#include <thread>





void SocketThreadLeader::SetNumberOfSocketLists(int pNumberOfSocketLists)
{
    if( pNumberOfSocketLists<1 )
    {
        NumberOfSocketLists=1;
    }
    else
    {
        NumberOfSocketLists=pNumberOfSocketLists;
        if( pNumberOfSocketLists > GetNumberOfCPUCores() )
        {
            printf( "WARNING : NumberOfSocketLists exceeds NumberOfCPUCores(%i). This might cause performance degradation.\n", GetNumberOfCPUCores() );
        }
    }
}

void SocketThreadLeader::GrowListCollection()
{
    while( SocketThreadCollection.size() < NumberOfSocketLists )
    {
        SocketThreadCollection.emplace_back();
    }
}

void SocketThreadLeader::ShrinkListCollection()
{
    int NumberToRemove;

    if( NumberOfSocketLists<1 )
    {
        NumberOfSocketLists = 1;
    }

    for (std::list<SocketThreadList>::reverse_iterator SocketCollectionIterator=SocketThreadCollection.rbegin();
        SocketThreadCollection.size()>NumberOfSocketLists || SocketCollectionIterator!=SocketThreadCollection.rend() ; ++SocketCollectionIterator)
    {
         SocketCollectionIterator->TagForDeletion();
    }
}

void SocketThreadLeader::CheckCollectionSize()
{
    int NumberOfListsCurrentlyInCollection =  SocketThreadCollection.size();

    if( NumberOfListsCurrentlyInCollection < NumberOfSocketLists )
    {
        GrowListCollection();
    }
    else if (  NumberOfListsCurrentlyInCollection > NumberOfSocketLists )
    {
        ShrinkListCollection();
    }
}

void SocketThreadLeader::PointToSmallestList()
{
//fprintf(stderr,"SocketThreadLeader::PointToSmallestList begins\n" );
    if( SocketThreadCollection.size() >0 )
    {
        SocketThreadList *CurrentSmallestList = &(*SocketThreadCollection.begin());
        for (std::list<SocketThreadList>::iterator SocketCollectionIterator=SocketThreadCollection.begin();
            SocketCollectionIterator!=SocketThreadCollection.end() ; ++SocketCollectionIterator)
        {
            SocketCollectionIterator->SetSmallest( false );

            if( SocketCollectionIterator->GetListSize() <  CurrentSmallestList->GetListSize() )
            {
                CurrentSmallestList = &(*SocketCollectionIterator);
            }
        }
        CurrentSmallestList->SetSmallest( true );
    }
//fprintf(stderr,"SocketThreadLeader::PointToSmallestList ends\n" );
}

void SocketThreadLeader::AddSocketEntityToQueue(SocketEntity pSocket)
{
fprintf(stderr, "SocketThreadLeader::AddSocketEntityToQueue( socket descriptor %i )\n", pSocket.GetDescriptor() );
    SocketQueue.push_back( pSocket );
fprintf(stderr, "SocketThreadLeader::AddSocketEntityToQueue ends\n" );
}


void SocketThreadLeader::ServiceCollection() {
fprintf(stderr, "SocketThreadLeader::ServiceCollection begins\n" );
    
    while( State!=SocketThreadLeader_TERMINATE )
    {
        CheckCollectionSize();
        PointToSmallestList();
        std::this_thread::sleep_for(std::chrono::milliseconds(CollectionServicingDelay));
    }
fprintf(stderr, "SocketThreadLeader::ServiceCollection ends\n" );
}


SocketThreadLeader::SocketThreadLeader()
{
fprintf(stderr, "SocketThreadLeader::SocketThreadLeader begins\n" );

    SetNumberOfSocketLists( GetNumberOfCPUCores()-1 );
    State=SocketThreadLeader_RUN;

    LeaderThread = std::thread([=] { ServiceCollection(); });

fprintf(stderr, "SocketThreadLeader::SocketThreadLeader ends\n" );
}

SocketThreadLeader::~SocketThreadLeader()
{
fprintf(stderr, "SocketThreadLeader::~SocketThreadLeader begins\n" );
    State=SocketThreadLeader_TERMINATE;
    SocketThreadCollection.erase(SocketThreadCollection.begin(),SocketThreadCollection.end());
    LeaderThread.join();
fprintf(stderr, "SocketThreadLeader::~SocketThreadLeader ends\n" );
}

SocketEntity SocketThreadLeader::GetHeadOfQueue()
{
fprintf(stderr, " SocketThreadLeader::GetHeadOfQueue begins\n" );
    SocketEntity *CurrentSocket = NULL;

    if( SocketQueue.size()==0 )
    {
        CurrentSocket = new SocketEntity( -1 );
    }
    else
    {
        
        std::list<SocketEntity>::iterator SocketQueueIterator=SocketQueue.begin();
        CurrentSocket = &(*SocketQueueIterator);
        SocketQueue.pop_front();
    }

fprintf(stderr, " SocketThreadLeader::GetHeadOfQueue ends\n" );
    return *CurrentSocket;
}

int SocketThreadLeader::GetMaxToPullFromQueue()
{
    return MaxToPullFromQueue;
}

int SocketThreadLeader::GetQueueSize()
{
    return SocketQueue.size();
}

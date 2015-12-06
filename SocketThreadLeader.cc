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

void SocketThreadLeader::IDSocketLists()
{
    //The lists *should* always be number smallest to largest from the beginning to the end of the list,
    //    BUT it's not guaranteed that items in the list will never be rearranged so we'll make two passes:
    //    the first to get the max and the second to assign the numbers. we could store this number in the
    //    SocketThreadLeader but then we'd have to maintain it seperately and considering how seldom this
    //    operation *should* be used it seems not worth the risk/effort.

    int  NextSocketID = 1;

    for (std::list<SocketThreadList>::iterator SocketCollectionIterator=SocketThreadCollection.begin();
        SocketCollectionIterator!=SocketThreadCollection.end() ; ++SocketCollectionIterator)
    {
        int CurrentSocketID = (*SocketCollectionIterator).GetSocketListID();
        if( CurrentSocketID>=NextSocketID )
        {
            NextSocketID=CurrentSocketID+1;
        }
    }

    for (std::list<SocketThreadList>::iterator SocketCollectionIterator=SocketThreadCollection.begin();
        SocketCollectionIterator!=SocketThreadCollection.end() ; ++SocketCollectionIterator)
    {
        int CurrentSocketID = (*SocketCollectionIterator).GetSocketListID();
        if( CurrentSocketID==0 )
        {
            (*SocketCollectionIterator).SetSocketListID(NextSocketID);
            NextSocketID++;
        }
    }
}

void SocketThreadLeader::GrowListCollection()
{
    while( SocketThreadCollection.size() < NumberOfSocketLists )
    {
        SocketThreadCollection.emplace_back();
    }

    IDSocketLists();
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
         (*SocketCollectionIterator).TagForDeletion();
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

void SocketThreadLeader::AddSocketEntityToQueue(SocketEntity *pSocket)
{
fprintf(stderr, "SocketThreadLeader::AddSocketEntityToQueue( socket descriptor %i )\n", pSocket->GetDescriptor() );
    SocketQueue.push_back( pSocket );
fprintf(stderr, "SocketThreadLeader::AddSocketEntityToQueue ends\n" );
}

void SocketThreadLeader::AddSocketEntityToUnusedQueue(SocketEntity *pSocket)
{
fprintf(stderr, "SocketThreadLeader::AddSocketEntityToUnusedQueue\n" );
    UnusedSocketQueue.push_back( pSocket );
fprintf(stderr, "SocketThreadLeader::AddSocketEntityToUnusedQueue ends\n" );
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

    for (std::list<SocketEntity*>::iterator SocketEntityIterator=UnusedSocketQueue.begin();
        SocketEntityIterator!=UnusedSocketQueue.end() ; ++SocketEntityIterator)
    {
         delete (*SocketEntityIterator);
    }
    UnusedSocketQueue.erase(UnusedSocketQueue.begin(), UnusedSocketQueue.end());

    LeaderThread.join();
fprintf(stderr, "SocketThreadLeader::~SocketThreadLeader ends\n" );
}

SocketEntity *SocketThreadLeader::GetUnusedSocketEntity()
{
    SocketEntity *CurrentSocket = NULL;

    if( UnusedSocketQueue.size()==0 )
    {
fprintf(stderr, "SocketThreadLeader::GetUnusedSocketEntity creating new SocketEntity\n" );
        char buffer[]= { "0.0.0.0" };
        CurrentSocket = new SocketEntity( -1, buffer );
    }
    else
    {
fprintf(stderr, "SocketThreadLeader::GetUnusedSocketEntity recycling SocketEntity\n" );
        CurrentSocket =  *UnusedSocketQueue.begin();
        UnusedSocketQueue.pop_front();
    }
    return CurrentSocket;
}

SocketEntity *SocketThreadLeader::GetHeadOfQueue()
{
fprintf(stderr, "SocketThreadLeader::GetHeadOfQueue begins\n" );
    SocketEntity *CurrentSocket = NULL;

    if( SocketQueue.size()==0 )
    {
        CurrentSocket = GetUnusedSocketEntity();
    }
    else
    {
        CurrentSocket =  *(SocketQueue.begin());
        SocketQueue.pop_front();
    }

fprintf(stderr, "SocketThreadLeader::GetHeadOfQueue ends\n" );
    return CurrentSocket;
}

int SocketThreadLeader::GetMaxToPullFromQueue()
{
    return MaxToPullFromQueue;
}

int SocketThreadLeader::GetQueueSize()
{
    return SocketQueue.size();
}

#Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
#Contributors include : Joe A Lyon

#include "SocketThreadLeader.h"
#include "SocketEntity.h"
#include "Utility.h"
#include <iostream>
#include <thread>





SocketThreadLeader::SocketThreadLeader()
{
//fprintf(stderr, "SocketThreadLeader::SocketThreadLeader begins\n" );

    SetNumberOfSocketLists( GetNumberOfCPUCores()-1 );
    State=SocketThreadLeader_RUN;

    LeaderThread = std::thread([=] { ServiceCollection(); });

//fprintf(stderr, "SocketThreadLeader::SocketThreadLeader ends\n" );
}

SocketThreadLeader::~SocketThreadLeader()
{
//fprintf(stderr, "SocketThreadLeader::~SocketThreadLeader begins\n" );
    State=SocketThreadLeader_TERMINATE;

    SocketThreadCollection.erase(SocketThreadCollection.begin(),SocketThreadCollection.end());

    LeaderThread.join();
//fprintf(stderr, "SocketThreadLeader::~SocketThreadLeader ends\n" );
}

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

void SocketThreadLeader::ConfigureSocketLists()
{
    int  NextSocketID = 1;

    for (std::list<SocketThreadList>::iterator SocketCollectionIterator=SocketThreadCollection.begin();
        SocketCollectionIterator!=SocketThreadCollection.end() ; ++SocketCollectionIterator)
    {
        int CurrentSocketID = (*SocketCollectionIterator).GetSocketListID();
        if( CurrentSocketID==0 )
        {
            (*SocketCollectionIterator).SetSocketListID(NextSocketID);
            NextSocketID++;
            (*SocketCollectionIterator).SetLeader(this);
        }
    }
}

void SocketThreadLeader::GrowListCollection()
{
    while( SocketThreadCollection.size() < NumberOfSocketLists )
    {
        SocketThreadCollection.emplace_back();
        std::list<SocketThreadList>::iterator tLast = SocketThreadCollection.end()--;
        tLast->SetLeader( this );
    }

    ConfigureSocketLists();
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

void SocketThreadLeader::ShowCollectionStatus()
{
    if( SocketThreadCollection.size() >0 )
    {
//fprintf(stderr,"SocketThreadLeader::Collection Status : ");
        for (std::list<SocketThreadList>::iterator SocketCollectionIterator=SocketThreadCollection.begin();
            SocketCollectionIterator!=SocketThreadCollection.end() ; ++SocketCollectionIterator)
        {
            //fprintf(stderr,"%3i ", (*SocketCollectionIterator).GetListSize());
        }
//fprintf(stderr,"\n");
    }
}

void SocketThreadLeader::ServiceCollection() {
//fprintf(stderr, "SocketThreadLeader::ServiceCollection begins\n" );
    
    while( State!=SocketThreadLeader_TERMINATE )
    {
        CheckCollectionSize();
        PointToSmallestList();
        ShowCollectionStatus();
        std::this_thread::sleep_for(std::chrono::milliseconds(CollectionServicingDelay));
    }
//fprintf(stderr, "SocketThreadLeader::ServiceCollection ends\n" );
}


int SocketThreadLeader::GetMaxToPullFromQueue()
{
    return MaxToPullFromQueue;
}

void SocketThreadLeader::NotifyOfTermination()
{
}

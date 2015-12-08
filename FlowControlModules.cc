#include "FlowControlModules.h"





DataModule *DataModule::GetNext()
{
    return Next;
}

void DataModule::ClearNext()
{
    Next = NULL;
}

void DataModule::SetNext( DataModule *pData)
{
    Next = pData;
}

void DataModule::AppendToEndOfList(DataModule *pData)
{
    pData->ClearNext();

    if( Next==NULL )
    {
        Next = pData;
    }
    else
    {
        AppendToEndOfList(pData);
    }
}

void DataModule::InsertAfterThisNode(DataModule *pData)
{
    if( pData != NULL )
    {
        DataModule *PrevData = Next;
        Next = pData; 
        Next->AppendToEndOfList( PrevData ); //Note that this does allow insertion of an entire sublist.
    }
}

void DataModulePool::Push_back( DataModule *pData )
{
    Gate.lock(); 

        //note that we do ensure that the DataModule controlled field is cleared in order to protect the 
        //    integrity of the list, but we do not touch the rest of the object. That *should* be explicitly
        //    cleared by the ControlModule passing the data to the pool. caveat emptor.
        pData->ClearNext();
    
        if( PoolHead==NULL )
        {
            PoolHead=pData;
            PoolTail=pData;
        }
        else
        {
            PoolTail->SetNext(pData);
            PoolTail=pData;
        }

    Gate.unlock(); 
}

DataModule *DataModulePool::Pop_front()
{
    Gate.lock();

        DataModule *tData;

        if( PoolHead!=NULL )
        {
            tData = PoolHead;
            PoolHead = tData->GetNext();
            tData->ClearNext();
        }
        else
        {
            //what we SHOULD do here is return a new, blank object...but I'm not sure how to do that yet,
            //    so I'm going to do the old fashioned and VERY VERY UNSAFE AND UNCLEAN THING AS A PLACEHOLDER ONLY!!!

            tData = NULL;
        }

    Gate.unlock();

    return tData;
}





void PluginModule::SetNextPluginModule( PluginModule *pNextPluginModule)
{
    NextPluginModule = pNextPluginModule;
}





void PluginModule::ReceiveData( DataModule *pData )
{
};

DataModule *PluginModule::ProvideData()
{
};

void PluginModule::NotifyOfTermination()
{
};


//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon

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





DataModulePool::DataModulePool()
{
    Gate = new std::mutex;
}

DataModulePool::~DataModulePool()
{
    DataModule *CurrentData = NULL;

    delete Gate;

    while( PoolHead!=NULL )
    {
        CurrentData = PoolHead;
        PoolHead = PoolHead->GetNext();
        delete CurrentData;
    }
}

void DataModulePool::Push_back( DataModule *pData )
{
std::lock_guard<std::mutex> lock(*Gate);

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
}

DataModule *DataModulePool::Pop_front()
{
std::lock_guard<std::mutex> lock(*Gate);

        DataModule *tData;

        if( PoolHead!=NULL )
        {
            tData = PoolHead;
            PoolHead = tData->GetNext();
            tData->ClearNext();
        }
        else
        {
            if( DataProvider==NULL )
            {
                tData = NULL;
            }
            else
            {
                tData = DataProvider->ProvideData();
            }
        }

    return tData;
}

DataModule *DataModulePool::ProvideData()
{
    Pop_front();
}

void DataModulePool::SetProvider(DataModule *pProvider)
{
    DataProvider = pProvider;
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

void PluginModule::SetOutputPool(DataModulePool *pPool )
{
    DataOutputPool = pPool;
}

DataModulePool *PluginModule::GetOutputPool()
{
    return DataOutputPool;
}

void PluginModule::SetInputPool(DataModulePool *pPool )
{
    DataInputPool = pPool;
}

DataModulePool *PluginModule::GetInputPool()
{
    return DataInputPool;
}

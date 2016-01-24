//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon

#ifndef _FLOW_CONTROL_MODULES
#define _FLOW_CONTROL_MODULES

#include <vector>
#include <list>
#include <mutex>
#include <stdlib.h>
#include <string>





class DataModule     //a simple, singly-linked list, with all its frailties...and its efficiencies.
{
private :
    DataModule *Next = NULL;

public :
    virtual DataModule *GetNext() final; 
    virtual void ClearNext() final;
    virtual void SetNext(DataModule *pData) final;
    virtual void AppendToEndOfList(DataModule *pData) final; 
    virtual void InsertAfterThisNode(DataModule *pData) final;
    virtual DataModule *ProvideData() = 0;
};


class DataModulePool : public DataModule
{
private:
    DataModule *PoolHead = NULL;    
    DataModule *PoolTail = NULL;    
    DataModule *DataProvider = NULL;
    DataModule *DataConsumer = NULL;
    std::mutex *Gate;

public:
    DataModulePool();
    ~DataModulePool();
    virtual void Push_back(DataModule *p) final;
    virtual DataModule *Pop_front() final;
    void SetProvider( DataModule *pProvider );
    void SetConsumer( DataModule *pConsumer );
    DataModule *ProvideData();
};





class PluginModule
{
protected :
    PluginModule *NextPluginModule = NULL;
    DataModulePool *DataInputPool = NULL;
    DataModulePool *DataOutputPool = NULL;

public :
    virtual void ReceiveData( DataModule *pData );
    virtual DataModule *ProvideData();

    virtual void NotifyOfTermination();

    void SetNextPluginModule( PluginModule *pNextPluginModule);
    void SetOutputPool( DataModulePool *pDataPool );
    DataModulePool *GetOutputPool();
    void SetInputPool( DataModulePool *pDataPool );
    DataModulePool *GetInputPool();

    void LogMessage( int LogLevel, std::string ClassAndMethodName, std::string MessageString );
};

// use the standard *nix log levels and definitions plus one

#define LOG_TERMINAL   0        //system terminating
#define LOG_EMERG      1        //system is unusable
#define LOG_ALERT      2        //action must be taken immediately
#define LOG_CRIT       3        //critical conditions
#define LOG_ERR        4        //error conditions
#define LOG_WARNING    5        //warning conditions
#define LOG_NOTICE     6        //normal, but significant, condition
#define LOG_INFO       7        //informational message
#define LOG_DEBUG      8        //debug-level message

#endif

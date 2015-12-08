#ifndef _FLOW_CONTROL_MODULES
#define _FLOW_CONTROL_MODULES

#include <vector>
#include <list>
#include <mutex>
#include <stdlib.h>





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
};


class DataModulePool
{
private:
    DataModule *PoolHead = NULL;    
    DataModule *PoolTail = NULL;    
    std::mutex Gate;

public:
    virtual void Push_back(DataModule *p) final;
    virtual DataModule *Pop_front() final;
};





class PluginModule
{
protected :
    PluginModule *NextPluginModule = NULL;

public :
    virtual void ReceiveData( DataModule *pData );
    virtual DataModule *ProvideData();

    virtual void NotifyOfTermination();

    void SetNextPluginModule( PluginModule *pNextPluginModule);

};

extern std::vector<PluginModule *> ChainOfControl;






#endif

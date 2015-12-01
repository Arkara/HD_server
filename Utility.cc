#include "Utility.h"





int GetNumberOfCPUCores()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

unsigned GetNumberOfThreadsSupported()
{
    return std::thread::hardware_concurrency();
}

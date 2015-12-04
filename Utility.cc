#include "Utility.h"





int GetNumberOfCPUCores()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

unsigned GetNumberOfThreadsSupported()
{
    return std::thread::hardware_concurrency();
}

long GetTimeInMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();
    return value.count();
}

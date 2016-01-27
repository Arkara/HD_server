//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon

#ifndef _HD_Utility

#include <unistd.h>
#include <thread>
#include <string.h>
#include <string>
#include <vector>



int GetNumberOfCPUCores();

unsigned GetNumberOfThreadsSupported();

long GetTimeInMilliseconds();

std::vector<std::string> *stringParse( char* rawString, char* splitOn);



#define _HD_Utility
#endif

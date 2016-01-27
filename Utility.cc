//Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
//Contributors include : Joe A Lyon

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

std::vector<std::string> *stringParse( char* rawString, char* splitOn)
{
    char *tokenStart = NULL,
         *tokenEnd = NULL,
         *rawStringEnd = NULL;

    rawStringEnd = rawString+strlen(rawString);

    std::vector<std::string>  *words=NULL;
    char wordBuffer[1024];
    int  wordLength=0;

    for( tokenStart = rawString; tokenStart < rawStringEnd; tokenStart+=(wordLength+1))
    {
        wordLength = strcspn( tokenStart, splitOn );
        if( wordLength>0 )
        {
            if( wordLength>1024 )
            {
               wordLength=1023;
            }
            sprintf( wordBuffer, "%*.*s", wordLength, wordLength, tokenStart );
            if( words==NULL )
            {
                words = new  std::vector<std::string>();
            }
            words->push_back( std::string( wordBuffer ));
        }
    }

    return words;
}

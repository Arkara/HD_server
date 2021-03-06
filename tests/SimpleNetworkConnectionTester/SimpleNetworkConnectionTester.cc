#Copyright (C) 2015, Harikan Dawn Associates. All Rights Reserved.
#Contributors include : Joe A Lyon

#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <thread>

int GetRandScaled(int scale)
{
    double R=rand();
    R/=RAND_MAX;
    R*=scale;
printf( "GetRandScaled returns %i\n", (int)R );
    return (int)R;
}

int main(int argc, char* argv[])
{
    srand (time(NULL));


    //wait a random number of seconds from 0-100 before starting
    sleep(GetRandScaled(10));
    

    int status;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof host_info);

    std::cout << "Setting up the structs..."  << std::endl;

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    
    
    char *machineIP;
    if(argc == 2) {
        char *argument = (char*)malloc(strlen(argv[1]) +1);
        strcpy(argument, argv[1]);
        
        machineIP = argument;
    }
    
    else {
        machineIP = (char*)malloc(sizeof(100));
        FILE *machineIPfile = popen("ip route get 1 | head -1 | cut -d' ' -f8", "r"); //Get IP
        fgets(machineIP,100,machineIPfile);
        
    }
    
    status = getaddrinfo( machineIP, "56124", &host_info, &host_info_list);
    if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;


    std::cout << "Creating a socket..."  << std::endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (socketfd == -1)  std::cout << "socket error " ;


    std::cout << "Connect()ing..."  << std::endl;
    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)  std::cout << "connect error" ;

    char msg[128];
    char incomming_data_buffer[1024];
    ssize_t bytes_recieved;
    int countdown = GetRandScaled(500);

    sprintf( msg, "Hello from Simple network tester %i", rand() );
    printf ("send()ing message 1 of %i : %s\n", countdown, msg );

    for( ; countdown>0; countdown-- ) //do between 1 and 500 transactions
    {
        int len;
        ssize_t bytes_sent;
        len = strlen(msg);
        bytes_sent = send(socketfd, msg, len, 0);

        std::cout << "Waiting to recieve data..."  << std::endl;
        bytes_recieved = recv(socketfd, incomming_data_buffer,1024, MSG_DONTWAIT);
        if (bytes_recieved == 0) 
        {
            std::cout << "host shut down." << std::endl ;
            break;
        }
        if(  incomming_data_buffer[0]==1 && incomming_data_buffer[1]==1 )
        {
            printf( "Got Heartbeat\n" );
        }

        if( GetRandScaled(100) >70 )
        {
            msg[0]=1;
            msg[1]=1;
            msg[2]=0;
            printf( "sending Heartbeat\n" );
        }
        else
        {
            sprintf( msg, "message %i", rand() );
            printf( "sending \"%s\"\n", msg );
        }
        if( GetRandScaled(100) <95 )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds( GetRandScaled(100)));
        }
        else
        {
            int WaitSeconds =  GetRandScaled(20);

            printf( "Waiting %i seconds\n", WaitSeconds );
            sleep( WaitSeconds);
        }
    }

    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
    std::cout << bytes_recieved << " bytes recieved :" << std::endl ;

    incomming_data_buffer[bytes_recieved] = '\0' ;
    std::cout << incomming_data_buffer << std::endl;
    std::cout << "Receiving complete. Closing socket..." << std::endl;
    freeaddrinfo(host_info_list);
    shutdown(socketfd,SHUT_RDWR);

}


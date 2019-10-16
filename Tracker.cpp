#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include<bits/stdc++.h>

using namespace std;


int main()
{
    int serverSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage cli;
    
    socklen_t addr_size;
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);  //Create Socket

    //Binding Socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8005);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    int status = bind(serverSocket , (struct sockaddr*)&serverAddr , sizeof(serverAddr));

    if(status < 0)
    {
       std::cout << "Binding Error" << std::endl;
       exit(1);
    }

    if(listen(serverSocket,5)==0)
        printf("Listening\n");
    else
        printf("Error\n");
    addr_size = sizeof(cli);
    char buffer[1024];
    int ClientSocketFd;
    int i=0;
    while(1)
    {
        bzero(buffer, 255);
        ClientSocketFd= accept(serverSocket, (struct sockaddr*)&cli, &addr_size);

        cout<<"Connection Established"<<endl;

        if(ClientSocketFd < 0)
        {
            printf("server acccept failed...\n"); 
		    exit(0);            
        }
        else
		    printf("server acccept the client...\n"); 
        bzero(buffer, 1024);
        int n = recv(ClientSocketFd, buffer, 1024*sizeof(char), 0);  //returns len of message in Bytes

        cout<<"Path reseived is : "<<buffer<<endl;

        FILE *fp = fopen(buffer, "rb");

        fseek ( fp , 0 , SEEK_END);
  	    int size = ftell ( fp );
  	    rewind ( fp );

        send (ClientSocketFd , &size, sizeof(size), 0);

        char SendingBuffer[512] ; 
	    while ( ( n = fread( SendingBuffer , sizeof(char) , 512 , fp ) ) > 0  && size > 0 ){
		    send (ClientSocketFd , SendingBuffer, n, 0 );
   	 	    memset ( SendingBuffer , '\0', 512);
		    size = size - n ;
        }
        fclose ( fp );
        close(ClientSocketFd);
        close(serverSocket);
    }
    return 0;
}
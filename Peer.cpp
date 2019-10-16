#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<iostream>
#include<pthread.h>
#include<netdb.h>
#include<ctype.h>
#include<iostream>
using namespace std;

void error(char * text)
{
perror(text);
exit(1);
}
void *sendfile(void* newfd1)
{
int sockfd, portno,n;
char buffer[1000];
int newsocketfd = *((int*)newfd1);
struct sockaddr_in serv_addr, cli_addr;
   
char temp[500];
recv(newsocketfd,&temp,sizeof(temp),0);
   
FILE *fp;
fp = fopen(temp,"r");   
if(fp==NULL)
{
cout<<"file not present";
exit(1);
}
fp = fopen("newfile.txt","rb");
fseek(fp,0,SEEK_END);
int size = ftell(fp);
rewind(fp);
send(newsocketfd,&size,sizeof(size),0);
n = fread(buffer,sizeof(char),sizeof(buffer),fp);
while(n>0 && size>0)
{
send(newsocketfd,buffer,n,0);
memset(buffer,'\0',sizeof(buffer));
size= size-n;
}
close(newsocketfd);
close(sockfd);
return 0;
}
void *serverthread(void* port)
{
int sockfd, newfd, portno,n;
char buff[1000];
struct sockaddr_in serv_addr, cli_addr;
socklen_t clilen;
sockfd=socket(AF_INET,SOCK_STREAM,0);
if(sockfd < 0)
error("error in socket creation");    
bzero((char*)&serv_addr,sizeof(serv_addr));    
portno=atoi((char*)port);
serv_addr.sin_family= AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(portno);
if (bind(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr))<0)
error("ERROR in bind");
listen(sockfd,5);    
clilen = sizeof(cli_addr);
while(1)
{
newfd=accept(sockfd,(struct sockaddr*) &cli_addr,&clilen);   
if(newfd<0)       
error("ERROR on accept");       
pthread_t newthread;
pthread_create(&newthread,NULL, sendfile ,(void*) &newfd);
pthread_join(newthread,NULL);

    }
}

int main(int argc, char* argv[])
{
pthread_t newthread1;
pthread_create(&newthread1,NULL, serverthread ,(void*) argv[3]);   
int socketfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buff[250];
if(argc<4)
{
fprintf(stderr,"hostname or portno missing %s\n",argv[0]);
exit(1);
}    
portno=atoi(argv[2]);
socketfd=socket(AF_INET,SOCK_STREAM,0);
if(socketfd<0)
error("error in socket");
server = gethostbyname(argv[1]);
if(server== NULL)
fprintf(stderr,"invalid host");
bzero((char *)&serv_addr,sizeof(serv_addr));
serv_addr.sin_family = AF_INET;     
bcopy((char *)server->h_addr_list[0],(char*)&serv_addr.sin_addr.s_addr,server->h_length);     
serv_addr.sin_port=htons(portno);
if(connect(socketfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
error("error while connecting");
bzero(buff,sizeof(buff));
    
    
FILE *f;
char temp[500]="newfile.txt";
send(socketfd,temp,sizeof(temp),0);
f=fopen("test.txt","wb");
int filesize;
recv(socketfd, &filesize,sizeof(filesize),0);
while ((n = recv(socketfd,buff,sizeof(buff),0))>0 && filesize > 0)
{        
fwrite(buff,sizeof(char),n,f);
fflush(f);
memset(buff,'\0',sizeof(buff));
filesize =filesize-n;
}
pthread_join(newthread1,NULL);
fclose(f);
close(socketfd);
return 0;
}
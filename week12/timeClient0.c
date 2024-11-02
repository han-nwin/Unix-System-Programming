// client0.c connects to server listening port# 12345

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#define PORT 22719

int main(int argc, char *argv[])
{
    char ip[] = "127.0.0.1";  // default IP of the server
    //int port = 12345;         // default port# of the server
    argv[1] = ip;

    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 


    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 
    serv_addr.sin_port = htons(PORT); 

    system(" date; hostname; whoami ");
    system(" netstat -aont | grep \":2271[0-9]\"");


    printf("\n timeClient: connecting to 127.0.0.1 Port# = %d \n", PORT);


    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    printf("\n timeClient: connected to timeServer. \n");
    system("ps");
    system(" netstat -aont | grep \":2271[0-9]\"");
    printf("\n\n");

    printf("\n timeClient: reading from timeServer. \n");
    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
	   exit(0);
    } 

    printf("\n timeClient: now terminated. \n");
    return 0;
}

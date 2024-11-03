// Client2.c 

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


int main(int argc, char *argv[])
{
    if (argc != 3) {
      printf("Usage: %s <IPv4-address> <port-number>", argv[0]);
      return -1;
    } else {
      printf("Connecting to IP: %s Port: %s\n", argv[1], argv[2]);
    }

    system("echo server; date; hostname; whoami; ps; ls -l");
    
    int port = atoi(argv[2]);

    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 


    memset(recvBuff, 0,sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n**CLIENT**: Error - Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, 0, sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        perror("\n**CLIENT**: Invalid address / Address not supported\n");
        return 1;
    } 
    serv_addr.sin_port = htons(port); 

    printf("\n**CLIENT** Step 1: connecting to %s Port# = %d \n", argv[1], port);
    system("ps; netstat -aont | grep \"`hostname -i`:2271[0-9]\"");

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n**CLIENT**: Error - Connect Failed \n");
       return 1;
    } 
    printf("\n**CLIENT** Step 2: connected to Server2. \n");
    system("ps; netstat -aont | grep \"`hostname -i`:2271[0-9]\"");

    printf("\n**CLIENT**: reading from Server2. \n");
    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n**CLIENT**: Error - Fputs error\n");
        }
    } 

    if(n < 0)
    {
      printf("\n**CLIENT**: Read error \n");
	    exit(0);
    } 
    printf("\n**CLIENT** Step 3: Client disconnected\n");
    system("ps; netstat -aont | grep \"`hostname -i`:2271[0-9]\"");
    printf("\n**CLIENT**: client ends! \n");
    return 0;
}

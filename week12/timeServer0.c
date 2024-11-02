// server0.c - listening port# 12345

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

#define PORT 22719

int check_running_server (char * prog_name, int curr_pid){
  int return_val = 0;

  //export previous running server to a temp file
  char command[100];
  snprintf(command, sizeof(command), "ps -f | grep -v %d | grep -v grep | grep \' %s\' > .server_check.temp", curr_pid, prog_name);
  system(command);
  
  //open temp file
  FILE *fp = fopen(".server_check.temp", "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
  
    //Extracting PID from .server_check.temp
  char line[1024];
  while(fgets(line, sizeof(line), fp) != NULL) {
    int pid;
    sscanf(line, "%*s %d", &pid);
    printf("\n**timeServer: Found running server PID: %d\n", pid);
    printf("\n**timeServer: Killing running server PID: %d...\n", pid);

    kill(pid, SIGKILL);
    return_val = 1;
  }

  fclose(fp);
  system("rm -f .server_check.temp"); //delete temp file

  return return_val;

}

int main(int argc, char *argv[])
{   
    int curr_pid = getpid();
    if (check_running_server(argv[0], curr_pid) == 1) {
      sleep(10);
      printf("\n**timeServer: Waiting for port to free before connecting (20s)...\n");
      sleep(20);
    }//check and kill previous server


    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(sendBuff, 0, sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
      perror("bind failed\n");
      return -1;
    } 

    if(listen(listenfd, 10) == -1) {
      perror("listen\n");
      return -1;
    }
    sleep(1);   
    printf("\n**timeServer: Server is up and listening through Port %d...\n", PORT);
  

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        if(connfd == -1){
          perror("accept");
          return -1;
        }
        printf("\n**timeServer: A client connected \n");
        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));

        printf("\n**timeServer: Writing to client...\n");
        if(write(connfd, sendBuff, strlen(sendBuff)) == -1) {
          perror("write\n");
          return -1;
        }
        
        close(connfd);
        printf("\n**timeServer: Client disconnected \n\n");
        sleep(1);
     }
}

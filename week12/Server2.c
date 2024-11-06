// Server2.c

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


//return 1 if there's a previous server running. 0 otherwise
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
    printf("\n**SERVER**: Found running server PID: %d\n", pid);
    printf("\n**SERVER**: Killing running server PID: %d...\n", pid);

    kill(pid, SIGKILL);
    return_val = 1;
  }

  fclose(fp);
  system("rm -f .server_check.temp"); //delete temp file

  return return_val;

}

void handle_sigalrm(int sig){
  printf("**SERVER**: TIME UP !!! Terminating server... \n");
  printf("**SERVER**: server ends\n");
  exit(0);
}

int main(int argc, char *argv[])
{  
    signal(SIGALRM, handle_sigalrm);
    if (argc != 3) {
      printf("Usage: %s <port-number> <time-duration>\n", argv[0]);
      return -1;
    } else {
      printf("\n**SERVER**: Binding to Port: %s, Wait time: %s\n", argv[1], argv[2]);
    }
    
    int port = atoi(argv[1]);
    int time_to_live = atoi(argv[2]);
    
    int curr_pid = getpid();
    if (check_running_server(argv[0], curr_pid) == 1) {
      printf("\n**SERVER**: Waiting for port to free before connecting...\n");
      
    //Wait until the port is free using a temp file
      int is_not_empty = 1;
      while(is_not_empty) {
        char command[100];
        snprintf(command, sizeof(command), "netstat -aont | grep \"%d\" > .check_port.temp", port);
        system(command);
        FILE *fp = fopen(".check_port.temp", "r");
        if (fp == NULL) {
            perror("File open failed");
            return -1; // Error in accessing the file
        }

        is_not_empty = (fgetc(fp) != EOF); // Check if first character is EOF
        fclose(fp);

        if(is_not_empty){
          printf("Port %d still in use...\n", port);
          sleep(5);
        }
      }
      system("rm -f .check_port.temp"); //delete temp file
      printf("\n**SERVER**: Done waiting! Port %d is freed and ready to bind\n", port);
      //sleep(60);
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
    serv_addr.sin_port = htons(port); 

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
      perror("bind failed\n");
      return -1;
    } 

    if(listen(listenfd, 10) == -1) {
      perror("listen\n");
      return -1;
    } 
    printf("\n**SERVER**: Server is up and listening through Port %d...\n", port);
    
    system("ps");
    char command[100];
    snprintf(command, sizeof(command), "netstat -aont | grep \"%d\"", port);
    system(command);

    while(1)
    {   
        alarm(time_to_live);
        printf("\n**SERVER** Server will stay for %d seconds then terminate if no client connected\n", time_to_live);
        printf("\n**SERVER** Step 1: Waiting for a client...\n");
        system("ps; netstat -aont | grep \"`hostname -i`:2271[0-9]\"");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        if(connfd == -1){
          perror("accept");
          return -1;
        }
        printf("\n**SERVER** Step 2: A client connected.\n");
        system("ps; netstat -aont | grep \"`hostname -i`:2271[0-9]\"");
        alarm(0);//reset alarm
        printf("\n**SERVER**: Wait time resetted");
                 
        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));

        printf("\n**SERVER**: Writing to client...\n");
        if(write(connfd, sendBuff, strlen(sendBuff)) == -1) {
          perror("write\n");
          return -1;
        }
        
        close(connfd);

        printf("\n**SERVER** Step 3: Client disconnected \n\n");
        system("ps; netstat -aont | grep \"`hostname -i`:2271[0-9]\"");
        sleep(1);
     }
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <number of child>\n", argv[0]);
		return 1;
	}
	printf("Name: Tan Han Nguyen\nNetID: TXN200004\nCourse/section: SE3377.001\nLab: Week 08 Lab 1\n");
	system("date;whoami;ps;ls -l"); //print header
	
	int caseNum = atoi(argv[1]); // capture argument to in
	printf("Case Number: %d\n", caseNum);
	
	pid_t ppid = getppid(); //Store the parent's parent ID to use later at the end
	switch(caseNum) {
              case 1:
                  fork();  // Creates 1 child process
                  printf("PID: %d, Parent PID: %d\n", getpid(), getppid());
                  break;

              case 2:
                  fork();
                  fork();
                  printf("PID: %d, Parent PID: %d\n", getpid(), getppid());
                  break;
              case 3:
                  fork();
                  fork();
                  fork();
                  printf("PID: %d, Parent PID: %d\n", getpid(), getppid());
                  break;

              case 4:
                  if (fork() && fork()) { 
                      fork(); 
                  }
                  if (fork() && fork()) { 
                      fork(); 
                  }
                  if (fork() && fork()) { 
                      fork(); 
                  }
                  printf("PID: %d, Parent PID: %d\n", getpid(), getppid());
                  break;

              case 5:
                  for (int i = 1; i < 5; i++) {
                      fork();  // Loop to fork 4 times
                  }
                  printf("PID: %d, Parent PID: %d\n", getpid(), getppid());
                  break;

              default:
                  fprintf(stderr, "Invalid argument (1 - 5)\n");
                  return 1;
          }
    // Wait for all child processes to finish
    while (wait(NULL) > 0);
  
    // Print termination statement
    if(getppid() == ppid){
        printf("End of tryFork for case #%d (PID: %d)\n", caseNum, getpid());
    }
    
    
    return 0;
}


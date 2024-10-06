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
	system("date;whoami;ps;ls -l"); //print header
	int caseNum = atoi(argv[1]); // capture argument to in
	
	int count = 1; //parent already run so count = 1 
	switch(caseNum) {
		case 1:
			pid_t pid = fork();
			if (pid < 0) {
				perror("Fork failed");
			}
			//Child process 
			else if (pid == 0) {
				printf("Child process: pid = %ld\n", (long)getpid());
				count++;
				exit(0);
			}
			//Parent process
			else {
				wait(NULL); //wait for the child to finish
				printf("Parent process: pid = %ld\n", (long)getpid());
				count ++;
				printf("Number of processes run: %d\n", count);
			}
			break;
		case 2:
			break;
		case 3:
			break;	
		case 4:
			break;
		case 5:
			break;
		default:
			fprintf(stderr, "Invalid argument (1 - 5)");
			return 1;
	}
	return 0;
}


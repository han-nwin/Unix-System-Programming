#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid, pid2;

    // First set of fork operations
    if ((pid = fork()) || (pid2 = fork())) {
        fork();
    }

    // Second set of fork operations
    if ((pid = fork()) || (pid2 = fork())) {
        fork();
    }

    // Third set of fork operations
//    if ((pid = fork()) && (pid2 = fork())) {
  //      fork();
    //}

    // Print the PID of each process created
    printf("Process ID: %d, Parent ID: %d\n", getpid(), getppid());

    return 0;
}


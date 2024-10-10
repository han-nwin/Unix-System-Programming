#include <stdio.h>
#include <unistd.h> // for fork()

int main() {
    int nprocs = 3; // You can change this value
    pid_t childpid;

    printf("Initial process ID: %d\n", getpid());

    for (int i = 1; i < nprocs; i++) {
        childpid = fork();
    // At this point, all processes have been created.
    // Pause to allow manual counting of processes
    }
    sleep(1); // Sleep to see the output and process count
    printf("PID: %d, PPID: %d\n", getpid(), getppid());
    return 0;
}


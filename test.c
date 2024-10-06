#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main() {
    pid_t pid;
    int process_count = 1;  // Start with the initial process (the parent)

    // First fork
    pid = fork();
    if (pid < 0) {
        perror("First fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process from first fork
        process_count = 2;  // After the first fork, there are 2 processes
    } else {
        // Parent process continues
    }

    // Second fork
    pid = fork();
    if (pid < 0) {
        perror("Second fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process from second fork
        process_count = 4;  // After the second fork, there are 4 processes
    } else {
        // Parent process continues
    }

    // Third fork
    pid = fork();
    if (pid < 0) {
        perror("Third fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process from third fork
        process_count = 8;  // After the third fork, there are 8 processes
    } else {
        // Parent process continues
    }

    // Print out process information
    printf("Process PID: %d, Parent PID: %d, Process Count: %d\n", getpid(), getppid(), process_count);

    // Sleep for a while to ensure all processes can print
    sleep(1);

    return 0;
}


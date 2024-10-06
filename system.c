#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
int system(const char *cmdstring) {
    pid_t pid;
    int status;

    if (cmdstring == NULL) {
        return 1;  // If command processor is available, return non-zero
    }

    // Fork a child process
    if ((pid = fork()) < 0) {
        status = -1;  // Error during fork
    } else if (pid == 0) {  // Child process
        // Execute the command string in a shell
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);  // If execl fails
    } else {  // Parent process
        // Wait for the child process to finish
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {  // Ignore EINTR errors
                status = -1;
                break;
            }
        }
    }

    return status;  // Return the status of the executed command
}
int main(int argc, char *argv[]) {
    int status;
    char command[1024] = {0};  // Allocate a buffer for the full command

    // Check if a command was provided as an argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        return 1; // Exit with error code
    }

    // Combine all arguments into one command string
    for (int i = 1; i < argc; i++) {
        strcat(command, argv[i]);  // Append each argument to the command string
        strcat(command, " ");      // Add a space between arguments
    }

    // Execute the combined command
    status = system(command);  // Execute the command
    if (status == -1) {
        perror("system");
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// Function to execute multiple commands with pipes
void execute_pipeline(char ***commands) {
    int i = 0;
    int pipefd[2];  // Pipe file descriptors
    int prev_pipefd[2]; // For holding the previous pipe's file descriptors
    pid_t pid;

    // Loop through all commands
    while (commands[i] != NULL) {
        // Create a pipe for all but the last command
        if (commands[i + 1] != NULL) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        // Fork the child process for the current command
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {  // Child process
            // If it's not the first command, redirect stdin to the read end of the previous pipe
            if (i > 0) {
                dup2(prev_pipefd[0], STDIN_FILENO);
                close(prev_pipefd[0]);
                close(prev_pipefd[1]);
            }

            // If it's not the last command, redirect stdout to the write end of the current pipe
            if (commands[i + 1] != NULL) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }

            // Execute the command
            if (execvp(commands[i][0], commands[i]) == -1) {
                perror("execvp");
                exit(1);
            }
        } else {  // Parent process
            // Close the previous pipe in the parent
            if (i > 0) {
                close(prev_pipefd[0]);
                close(prev_pipefd[1]);
            }

            // Move current pipe to previous for next iteration
            if (commands[i + 1] != NULL) {
                prev_pipefd[0] = pipefd[0];
                prev_pipefd[1] = pipefd[1];
            }

            // Wait for the child process to finish
            waitpid(pid, NULL, 0);
        }

        i++; // Move to the next command
    }
}

int main() {
    // Example: A pipeline of commands "ls", "grep txt", "wc -l"
    char *cmd1[] = {"ls", "/home/", NULL};
    char *cmd2[] = {"grep", "txt", NULL};
    char *cmd3[] = {"wc", "-l", NULL};

    // Array of command arrays
    char **commands[] = {cmd1, cmd2, cmd3, NULL};

    // Execute the pipeline
    execute_pipeline(commands);

    return 0;
}


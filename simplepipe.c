#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[100];

    // Create a pipe
    pipe(pipefd);

    // Fork the process
    pid = fork();

    if (pid == 0) {
        // Child process
        close(pipefd[1]);  // Close unused write end
        read(pipefd[0], buffer, sizeof(buffer));  // Read from pipe
        printf("Child received: %s\n", buffer);   // Print received message
        close(pipefd[0]);  // Close read end
    } else {
        // Parent process
        close(pipefd[0]);  // Close unused read end
        char message[] = "Hello from parent!";
        write(pipefd[1], message, strlen(message) + 1);  // Write to pipe
        close(pipefd[1]);  // Close write end
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 1024

int main(void) {
	int fd;
	int n;
	char buf[BUFSIZE];
	fd = open("open.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR );
	dup2(fd,STDOUT_FILENO);
	char buffer[1024];
        ssize_t bytesRead;

    // Read from stdin (normally it would come from the terminal)
        while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
          if (write(STDOUT_FILENO, buffer, bytesRead) < 0) {
            perror("Failed to write to file");
            close(fd);
            exit(EXIT_FAILURE);
          }
        }
	close(fd);
	exit(0);
}

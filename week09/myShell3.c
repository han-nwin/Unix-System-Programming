/* myshell3.c
 * My Shell program
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <strings.h> // for strcasecmp
#include <glob.h> // to expand wildcard
#include <fcntl.h> // for O_CREAT etc.
#include <sys/wait.h> // for waitpid
#include <sys/types.h> // for pid_t
#include <signal.h> // for signal handling 
#include <time.h> // for alarm 

// function to perform redirection: read from stdin -> read from file, write to stdout -> write to file
void file_redir(char* redirection, char* file) {
    int fdin; 
    int fdout;
    
    // redirect standard input
    if (!strcmp(redirection, "<")) {
        if ((fdin = open(file, O_CREAT | O_RDONLY)) < 0) {
            perror(file); // open failed
            return;  // exit the function if open fails
        }
        dup2(fdin, STDIN_FILENO); 
        close(fdin);
    }
    
    // redirect standard output
    if (!strcmp(redirection, ">")) {
        if ((fdout = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) { 
            perror(file);  // if open fails, print an error
            return;  // exit the function if open fails
        }
        dup2(fdout, STDOUT_FILENO);
        close(fdout);
    }
    return;
}

// function to parse line input into an array of arguments
void parse(char* line, char** argv) {
    // checking the line char by char until null
    while (*line != '\0') {
        // check if it's a whitespace -> replace it with '\0'
        while (*line == ' ' || *line == '\t' || *line == '\n') {
            *line++ = '\0'; // replace and move to next char
        }

        *argv++ = line; // assign non-whitespace char to *argv and move *argv to the next index
        
        // now skip the rest of the argument until the start of the next argument
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') {
            line++;
        }   
    }
    *argv = NULL; // mark the end of the argument array
}

// function to execute command using child process
void execute(char** argv) {
    pid_t pid;
    int status;

    // call a child process, then check its pid
    // pid < 0 = forking failed
    if ((pid = fork()) < 0) {
        printf("**error: forking child process failed \n");
        exit(1);
    }
    else if (pid == 0) {
        if (execvp(*argv, argv) < 0) {
            printf("**error: exec failed\n");
            exit(1);
        }
    }
    else {
        // waiting for the child's completion
        while (wait(&status) != pid);
    }
}

// function to expand wildcard
int expand_wildcards(char** argv, char* expanded_argv[]) {
    int i = 0;
    int j = 0;
    size_t k;

    // iterate through each argument in argv
    while (argv[i] != NULL) {
        glob_t globbuf;

        // apply glob to each argument
        if (glob(argv[i], 0, NULL, &globbuf) == 0) {
            for (k = 0; k < globbuf.gl_pathc; k++) {
                expanded_argv[j++] = strdup(globbuf.gl_pathv[k]); // expand wildcard
            }
            globfree(&globbuf); // free memory allocated by glob
        }
        else {
            // if no match, just copy the argument as is
            expanded_argv[j++] = strdup(argv[i]);
        }
        i++; // move to next argument
    }
    expanded_argv[j] = NULL; // null terminator at the end
    return j; // return number of expanded arguments
}

// custom alarm
pid_t alarm_pid = -1;
void my_alarm(int seconds) {
    // check if user wants to reset the alarm 
    if (seconds == 0) {
        // if there's a previous alarm (a child) -> kill it 
        if (alarm_pid > 0) {
            kill(alarm_pid, SIGKILL); // kill the alarm child process 
            waitpid(alarm_pid, NULL, 0);
            alarm_pid = -1; // reset the pid
            //Only print this if there's a previous alarm 
            printf("**alarm is off**\n");
        }
        return; //return if just call my_alarm(0) with no previous alarm set 
    }
    // set a new alarm 
    printf("alarm set for %d seconds.\n", seconds);
    alarm_pid = fork();
    if (alarm_pid == 0) {
        // child goes to sleep to wait 
        sleep(seconds);

        // after waking up, kill the parent and send a signal (SIGALRM) if the alarm is on 
        kill(getppid(), SIGALRM);
        // exit the child process after sending the signal and the parent is killed
        exit(0);
    }
}
//======================== Signal Handler ====================//
void mysig_handler(int signal) {
    printf("\n**This is a message from Week09 lab 2 - Signal Handler!**\n");
    if (signal == SIGALRM) {
        printf("Alarm Triggered! Shell terminated due to inactivity.\n");
        exit(0);  // exit the shell 
    } else {
        printf("Caught Signal: %d\n", signal);
    }
}

//=============================================================//
// define buffer size
#define max_line 4096
#define max_arg 64

int main(void) {
    // handle signal
    signal(SIGINT, mysig_handler);
    signal(SIGALRM, mysig_handler);

    char line[max_line];
    char line_backup[max_line];
    char* prompt = "{myShell3}$ ";
    char* argv[64]; // maximum 64 arguments
    char* expanded_argv[64]; // array to hold expanded wildcard arguments

    // first prompt
    printf("%s", prompt); // print the prompt
    // the loop
    while (fgets(line, sizeof(line), stdin) != NULL) {
        //Turn off the alarm because there's input 
        my_alarm(0);
        // remove \n from fgets
        line[strlen(line) - 1] = '\0';

        // backup the line before parsing
        strcpy(line_backup, line);  // create a backup of the original input
        // parse the line into an array of arguments
        parse(line, argv);

        // exit case
        if (strcasecmp(argv[0], "exit") == 0) {
            break;
        }
        // listall case
        else if (strcasecmp(argv[0], "listall") == 0) {
            argv[0] = "ls";
            argv[1] = "-la";
            argv[2] = NULL; // mark the end of the argv array
            execute(argv);
            printf("%s", prompt); // prompt after exec
            continue; // skip to next loop
        }
        // compile
        else if (strcasecmp(argv[0], "compile") == 0) {
            argv[0] = "gcc";
            // skip argv[1]
            argv[2] = "-o";

            // getting the output file name from argv[1]
            size_t name_length = strlen(argv[1]) - 2; // not getting .c
            char* output_name = strndup(argv[1], name_length); // duplicate the "program"
            argv[3] = output_name;

            argv[4] = NULL;

            // expand wildcards
            expand_wildcards(argv, expanded_argv);

            execute(expanded_argv); // execute
            printf("%s", prompt); // prompt again
            continue; // skip to next loop
        }
        // redirection case
        else if ((strchr(line_backup, '<') != NULL) || (strchr(line_backup, '>') != NULL)) {
            // variables to store file redirection paths
            char* input_file = NULL;
            char* output_file = NULL;
            char* argv2[64];  // array to hold the actual command without redirection parts
            int j = 0;  // index for argv2

            // parse the arguments and look for redirection
            for (int i = 0; argv[i] != NULL; i++) {
                if (strcmp(argv[i], "<") == 0) {
                    // input redirection: store the filename
                    input_file = argv[i + 1];
                    i++;  // skip the filename in the next iteration
                }
                else if (strcmp(argv[i], ">") == 0) {
                    // output redirection: store the filename
                    output_file = argv[i + 1];
                    i++;  // skip the filename in the next iteration
                }
                else {
                    // normal command argument: copy to argv2
                    argv2[j++] = argv[i];
                }
            }
            argv2[j] = NULL;  // null-terminate the argv2 array

            pid_t pid = fork(); // call a child process 
            if (pid == 0) { // child process redirects and executes the commands
                // handle input redirection
                if (input_file != NULL) {
                    file_redir("<", input_file); // call redirection function
                }

                // handle output redirection
                if (output_file != NULL) {
                    file_redir(">", output_file); // call redirection function
                }

                // expand wildcards and execute the command
                expand_wildcards(argv2, expanded_argv);
                execute(expanded_argv);
                exit(0);

            } else if (pid > 0) { // parent process waits for the child to end 
                wait(NULL); // wait 
                printf("%s", prompt); // prompt again
                continue;
            }
        }
        // ; case
        else if (strchr(line_backup, ';') != NULL) {
            char* command; // pointer to each individual command

            // use strtok() to split line_backup by ';', tokenize the first command
            command = strtok(line_backup, ";");

            while (command != NULL) {
                // trim leading and trailing spaces from the command
                while (*command == ' ') {
                    command++; // remove leading spaces by moving command pointer forward
                }
                char* end = command + strlen(command) - 1;
                while (end > command && *end == ' ') {
                    *end-- = '\0'; // remove trailing space then move the end pointer backward
                }

                // parse the command into arguments
                parse(command, argv);
                // expand wildcards
                expand_wildcards(argv, expanded_argv);
                // execute the command
                execute(expanded_argv);

                // get the next command in the sequence using NULL to tokenize the same string
                command = strtok(NULL, ";");
            }

            printf("%s", prompt); // prompt again
            continue;
        }
        // pipe case
        else if (strchr(line_backup, '|') != NULL) {
            // variable to use 2 pipes for multiple commands
            int i = 0;
            int pfd[2];  // pipe file descriptors
            int prev_pfd[2]; // for holding the previous pipe's file descriptors
            pid_t pid;
            char* command; // pointer to each individual command
            char* command_array[max_arg]; // array to store the command pointers
            int command_count = 0; // counter to keep track of command_array

            // use strtok() to split line_backup by '|', tokenize the first command
            command = strtok(line_backup, "|");

            while (command != NULL) {
                // trim leading and trailing spaces from the command
                while (*command == ' ') {
                    command++; // remove leading spaces by moving command pointer forward
                }
                char* end = command + strlen(command) - 1;
                while (end > command && *end == ' ') {
                    *end-- = '\0'; // remove trailing space then move the end pointer backward
                }

                // duplicate and store the command in the array and increment the counter
                command_array[command_count++] = strdup(command);

                // get the next command in the sequence using NULL to continue tokenizing the same string
                command = strtok(NULL, "|");
            }
            // null-terminate the array of commands
            command_array[command_count] = NULL;

            // loop through all commands
            while (command_array[i] != NULL) {
                // create a pipe for all but the last command (since the last one -> stdout)
                if (command_array[i + 1] != NULL) {
                    if (pipe(pfd) == -1) {
                        perror("pipe");
                        exit(1);
                    }
                }

                // fork the child process for the current command
                pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(1);
                }

                if (pid == 0) {  // child process
                    // if not the first command, redirect stdin to the read end of the previous pipe
                    if (i > 0) {
                        close(prev_pfd[1]); // close the write end of prev pipe
                        dup2(prev_pfd[0], STDIN_FILENO); // redirect stdin to read end of prev pipe
                        close(prev_pfd[0]); // close read end of prev pipe
                    }

                    // if not the last command, redirect stdout to the write end of the current pipe
                    if (command_array[i + 1] != NULL) {
                        close(pfd[0]); // close the read end of the current pipe
                        dup2(pfd[1], STDOUT_FILENO); // redirect stdout to write end of current pipe
                        close(pfd[1]); // close write end of current pipe
                    }

                    // parse, expand, and execute each command string
                    parse(command_array[i], argv);
                    expand_wildcards(argv, expanded_argv);
                    // note: not using execute() because it interferes with the child process here.
                    if (execvp(*expanded_argv, expanded_argv) < 0) {
                        printf("**error: exec failed\n");
                        exit(1);
                    }

                } else {  // parent process
                    // close the previous pipe in the parent
                    if (i > 0) {
                        close(prev_pfd[0]);
                        close(prev_pfd[1]);
                    }

                    // move the current pipe to previous, then start a new current pipe in the next iteration
                    if (command_array[i + 1] != NULL) {
                        prev_pfd[0] = pfd[0];
                        prev_pfd[1] = pfd[1];
                    }

                    // wait for the child process to finish
                    waitpid(pid, NULL, 0);
                }

                i++; // move to the next command
            }

            // free memory allocated by strdup
            for (int j = 0; j < command_count; j++) {
                free(command_array[j]);  // free each command duplicated by strdup
            }

            printf("%s", prompt); // prompt again
            continue;
        }
        // alarm case 
        else if (strcmp(argv[0], "alarm") == 0) {
            int seconds = atoi(argv[1]);
            if (seconds >= 0) {
                my_alarm(seconds);
            }
            else {
                printf("Invalid alarm time\n");
            }
            printf("%s", prompt);
            continue;
        }

        // expand wildcards
        expand_wildcards(argv, expanded_argv);
        execute(expanded_argv); // child execute other commands
        printf("%s", prompt); // prompt again
    }
    return 0;
}


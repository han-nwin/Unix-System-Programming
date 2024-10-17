/* myShell1.c
 * A simple shell program that uses child process and exec with redirection, process sequence and pipe.
 */
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <strings.h> //for strcasecmp
#include <glob.h> // to expand wildcard
#include <fcntl.h> // for O_CREAT etc.
#include <sys/wait.h> // for waitpid
#include <sys/types.h> // for pid_t

//Function to perform redirection read from stdin -> read from file, write to stdout -> write to file
void fileRedir(char* redirection, char* file){
      int fdin; 
      int fdout;
        //redirect standard input
       if(!strcmp(redirection, "<")){
           if ((fdin = open(file, O_CREAT | O_RDONLY)) < 0) {
              perror(file); /* open failed */
              return;  // Exit the function if open fails
            }
            dup2(fdin, STDIN_FILENO); 
            close(fdin);
       }
       //redirect standard output
       if(!strcmp(redirection, ">")){
            if ((fdout = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) { 
              perror(file);  // If open fails, print an error
              return;  // Exit the function if open fails
            }
            dup2(fdout, STDOUT_FILENO);
            close(fdout);
        }
        return;
}
// Function to parse line input into array of arguments
void parse(char* line, char** argv) {
	//Checking the line char by char until null
	while(*line != '\0'){
		//Check if it's a white space -> replace it with '\0'
		while (*line == ' ' || *line == '\t' || *line == '\n'){
			*line++ = '\0'; //replace and move to next char
		}
	
	*argv++ = line; // assign non-white space char to *argv and move *argv to next index
		//Now skip the rest of the argument until the start of next argument
		while(*line != '\0' && *line != ' '&& *line != '\t' && *line != '\n'){
			line++;
		}	
	}
	*argv = NULL; //Mark the end of the argument array
}


//Function to execute command using child process
void execute(char** argv){
	pid_t pid;
	int status;

	//Call a child process, then check its pid
	//pid < 0 = forking failed
	if ((pid = fork()) < 0){
		printf("**ERROR: Forking child process failed \n");
		exit(1);
	}
	else if (pid == 0){
		if(execvp(*argv, argv) < 0){
			printf("**ERROR: exec failed\n");
			exit(1);
		}
	}
	else{
		//Waiting for the child completion
		while(wait(&status) != pid);
	}
}

//Function to expand wildcard
int expand_wildcards(char** argv, char* expanded_argv[]) {
	int i = 0;
	int j = 0;
	size_t k;

	//Iterate through each argument in argv
	while (argv[i] != NULL){
		glob_t globbuf;

		//Apply glob to each argument
		if(glob(argv[i], 0, NULL, &globbuf) == 0){
			for(k = 0; k < globbuf.gl_pathc; k++){
				expanded_argv[j++] = strdup(globbuf.gl_pathv[k]); //Expand wildcard
			}
			globfree(&globbuf); //Free memory allocated by glob
		}
		else {
			//If no match, just copy the argument as is
			expanded_argv[j++] = strdup(argv[i]);
		
		}
	i++;//move to next argument
	}
	expanded_argv[j] = NULL; // null terminator at the end
	return j; //Return number of expanded arguments
	
}

//Define buffer size
#define MAX_LINE 4096
#define MAX_ARG 64

int main(void){
	char line[MAX_LINE];
	char line_backup[MAX_LINE];
	char* prompt = "{myShell2}$ ";
	char* argv[64]; // maximum 64 argument
	char* expanded_argv[64]; //Array to hold expanded wildcard arguments
	int has_redir = 0;
	
	//first prompt
	printf("%s", prompt); // print the prompt
	//The loop
	while(fgets(line, sizeof(line), stdin) != NULL){
		//Remove \n from fgets
		line[strlen(line) - 1] = '\0';
		
		// Backup the line before parsing
                strcpy(line_backup, line);  // Create a backup of the original input
		//Parse the line into array of arguments
		parse(line, argv);
      
                // Debugging output: show parsed arguments
                /*printf("Parsed arguments:\n");
                for (int i = 0; argv[i] != NULL; i++) {
                    printf("argv[%d] = %s\n", i, argv[i]);
                }*/

		//exit case
		if(strcasecmp(argv[0], "exit") == 0){
			break;
		}
		//listall case
		else if (strcasecmp(argv[0], "listall") == 0){
			argv[0] = "ls";
			argv[1] = "-la";
			argv[2] = NULL; //Mark the end of the argv array
			execute(argv);
			printf("%s", prompt); // prompt after exec
			continue; // skip to next loop
		}
		//compile
		else if (strcasecmp(argv[0], "compile") == 0){
			argv[0] = "gcc";
			//Skip argv[1]
			argv[2] = "-o";

			//Getting the output file name from argv[1]
			size_t name_length = strlen(argv[1]) - 2; // not getting .c
			char *output_name = strndup(argv[1], name_length); // Duplicate the "program"
			argv[3] = output_name;

			argv[4] = NULL;

			//expand wildcards
			expand_wildcards(argv, expanded_argv);

			execute(expanded_argv); // execute
			printf("%s", prompt); // Prompt again
			continue; //Skip to next loop
		
		}
		// < > case
		else if ((strchr(line_backup, '<') != NULL) || (strchr(line_backup, '>') != NULL)) {
                        // Variables to store file redirection paths
                        char* input_file = NULL;
                        char* output_file = NULL;
                        char* argv2[64];  // Array to hold the actual command without redirection parts
                        int j = 0;  // Index for argv2

                        // Parse the arguments and look for redirection
                        for (int i = 0; argv[i] != NULL; i++) {
                            if (strcmp(argv[i], "<") == 0) {
                                // Input redirection: store the filename
                                input_file = argv[i + 1];
                                i++;  // Skip the filename in the next iteration
                            }
                            else if (strcmp(argv[i], ">") == 0) {
                                // Output redirection: store the filename
                                output_file = argv[i + 1];
                                i++;  // Skip the filename in the next iteration
                            }
                            else {
                                // Normal command argument: copy to argv2
                                argv2[j++] = argv[i];
                            }
                        }
                        argv2[j] = NULL;  // Null-terminate the argv2 array
                        
                        pid_t pid = fork(); //call a child process 
                        if(pid == 0){// child process redirect and execute the commands
                            // Handle input redirection
                            if (input_file != NULL) {
                                fileRedir("<", input_file); // call redirect function
                            }

                            // Handle output redirection
                            if (output_file != NULL) {
                                fileRedir(">", output_file); // call redirect function
                            }

                            // Expand wildcards and execute the command
                            expand_wildcards(argv2, expanded_argv);
                            execute(expanded_argv);
                            exit(0);

                        } else if (pid > 0){//Parent process wait for child to end 
                            wait(NULL); //wait 
                            printf("%s", prompt); // Prompt again
                            continue;
                        }
                }
                // ; case
                else if (strchr(line_backup, ';') != NULL) {
                        char* command; // Pointer to each individual command

                        // Use strtok() to split line_backup by ';'. tokenize the first command
                        command = strtok(line_backup, ";");

                        while (command != NULL) {
                            // Trim leading and trailing spaces from the command
                            while (*command == ' ') {
                                command++; // Remove leading spaces by move command pointer forward
                            }
                            char *end = command + strlen(command) - 1;
                            while (end > command && *end == ' ') {
                                *end-- = '\0'; // Remove trailing space then move end pointer backward
                            }

                            // Parse the command into arguments
                            parse(command, argv);
                            // Expand wildcards
                            expand_wildcards(argv, expanded_argv);
                            // Execute the command
                            execute(expanded_argv);
                            
                            // Get the next command in the sequence use NULL to tokenize the same string
                            command = strtok(NULL, ";");
                        }

                        printf("%s", prompt); // Prompt again
                        continue;
                }
                // pipe case
                else if (strchr(line_backup, '|') != NULL){
                        //Variable to use 2 pipes for multiple commands
                        int i = 0;
                        int pfd[2];  // Pipe file descriptors
                        int prev_pfd[2]; // For holding the previous pipe's file descriptors
                        pid_t pid;
                        char* command; // Pointer to each individual command
                        char* command_array[MAX_ARG]; // Array to store the command pointers
                        int command_count = 0; // counter to keep track of command_array

                        // Use strtok() to split line_backup by ';'. tokenize the first command
                        command = strtok(line_backup, "|");

                        while (command != NULL) {
                            // Trim leading and trailing spaces from the command
                            while (*command == ' ') {
                                command++; // Remove leading spaces by move command pointer forward
                            }
                            char *end = command + strlen(command) - 1;
                            while (end > command && *end == ' ') {
                                *end-- = '\0'; // Remove trailing space then move end pointer backward
                            }

                            // Duplicate and store the command in the array and increment the counter
                            command_array[command_count++] = strdup(command);
                            
                            // Get the next command in the sequence using NULL to continue tokenizing the same string
                            command = strtok(NULL, "|");
                        }
                        // Null-terminate the array of commands
                        command_array[command_count] = NULL;

                        // Loop through all commands
                        while (command_array[i] != NULL) {
                            // Create a pipe for all but the last command. Since the last one -> stdout
                            if (command_array[i + 1] != NULL) {
                                if (pipe(pfd) == -1) {
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
                                //If not the first command, redirect stdin to the read end of the previous pipe
                                if (i > 0) {
                                    close(prev_pfd[1]); //close the write end of prev pipe
                                    dup2(prev_pfd[0], STDIN_FILENO); // redirect stdin to read end of prev pipe
                                    close(prev_pfd[0]); // close read end of prev pip
                                }

                                //If not the last command, redirect stdout to the write end of the current pipe
                                if (command_array[i + 1] != NULL) {
                                    close(pfd[0]); //close the read end of the curr pipe
                                    dup2(pfd[1], STDOUT_FILENO); //redirect stdout to write end of curr pipe
                                    close(pfd[1]); //close write end of curr pipe
                                }

                                //parse, expand, execute each command string
                                parse(command_array[i], argv);
                                expand_wildcards(argv, expanded_argv);
                                //NOTE: NOT using execute() because it interferes with the child process here.
                                if(execvp(*argv, argv) < 0){
			            printf("**ERROR: exec failed\n");
			            exit(1);
		                }
                                
                            } else {  //Parent process
                                //Close the previous pipe in the parent
                                if (i > 0) {
                                    close(prev_pfd[0]);
                                    close(prev_pfd[1]);
                                }

                                //Move current pipe to previous then start new curr pipe in the next iteration
                                if (command_array[i + 1] != NULL) {
                                    prev_pfd[0] = pfd[0];
                                    prev_pfd[1] = pfd[1];
                                }

                                // Wait for the child process to finish
                                waitpid(pid, NULL, 0);
                            }

                            i++; // Move to the next command
                        }
                        // Free memory allocated by strdup
                        for (int j = 0; j < command_count; j++) {
                                free(command_array[j]);  // Free each command duplicated by strdup
                        }
                        
                        printf("%s", prompt); // Prompt again
                        continue;
                }
                
                //OTHER CASES//
		//expand wildcards
		expand_wildcards(argv, expanded_argv);
		execute(expanded_argv); //child execute other commands
		printf("%s", prompt); //prompt again
	}
	return 0;

}


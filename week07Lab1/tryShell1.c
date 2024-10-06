/* tryShell1.c 
 * A simple shell program that uses child process an exec
 */
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> //for strcasecmp
#include <glob.h> // to expaln wildcard

// Function to parse line input into array of arguments
void parse(char *line, char**argv) {
	//Checking the line char by char until null
	while(*line != '\0'){
		//Check if it's a white space -> replace it with '\0'
		while (*line == ' ' || *line == '\t' || *line == '\n'){
			*line++ = '\0'; //replace and move to next char
		}
	
	*argv++ = line; // assign non-white space char to *argv and move *argb to next index
		//Now skip the rest of the argumen until the start of next argument
		while(*line != '\0' && *line != ' '&& *line != '\t' && *line != '\n'){
			line++;
		}	
	}
	*argv = NULL; //Mark the end of the argument array
}


//Function to execute command using child process
void execute(char **argv){
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
int expand_wildcards(char **argv, char *expanded_argv[]) {
	glob_t glob_results;
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
			globfree(&globbuf); //Free memory allocated by flob
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

int main(void){
	char line[MAX_LINE];
	char *prompt = "{tryShell1}$ ";
	char *argv[64]; // maximum 64 argument
	char *expanded_argv[64]; //Array to hold expanded wildcard arguments
	
	//first prompt
	printf("%s", prompt); // prinyt the prompt
	//The loop
	while(fgets(line, sizeof(line), stdin) != NULL){
		//Remove \n from fgets
		line[strlen(line) - 1] = '\0';
		
		//Parse the line into array of arguments
		parse(line, argv);

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
		//expand wildcards
		expand_wildcards(argv, expanded_argv);
		execute(expanded_argv); //child execute other commands

		printf("%s", prompt); //prompt again
	}
	return 0;

}


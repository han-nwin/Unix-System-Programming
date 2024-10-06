//myshell.c
//This program is a simple shell program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> //for strcmp()

int main() {
	char line[256];
	char prompt[] = "{myshell}$ ";

	//Print the first prompt
	printf("%s", prompt);

	//The loop
	while(fgets(line, sizeof(line), stdin) != NULL){
		//Remove \n from output of fgets
		line[strlen(line) - 1] = '\0';
		
		if (strcasecmp(line, "exit") == 0) {
			break;
		}
		else if(strcmp(line,"listall") == 0){
			system("ls -la");
		}
		else if(strcmp(line, "showme") == 0){
			system("w | grep \"txn\"");
		}
		else {
			system(line);
		}
	printf("%s", prompt); //Prompt again
	}
	return 0;
}

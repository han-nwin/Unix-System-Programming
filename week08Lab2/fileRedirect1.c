// Code sample - file redirection  
// C program to handle file redirection to set input.txt
// to be standard input: fileRedir("<", "input.txt");
//
 
void fileRedir(char * redirection, char* file){
 
      int fdout;

       if(!strcmp(redirection, "<")){
           if ((fdout = open(file, O_CREAT | O_RDONLY)) < 0) {
              perror(file); /* open failed */
       }
       dup2(fdout, 0); 
       }

// add code segment to handle standard output for 
// fileRedir(">", "output.txt");

}

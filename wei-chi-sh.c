#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


//reads a line of input from a file, dynamically allocates memory for it, and adjusts the allocated size as needed. It then returns a pointer to the resulting string.
//https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length

char *read_command_naive(void){
	int bufsize = 4;
	char *buf = malloc(sizeof(char) * bufsize);
	int pos = 0;


	while(1){
		char c = getchar();
		if(c !='\n'){
			//Need to allocate more space, doubling the memory every time as needed.
			if(pos == bufsize -1){
				bufsize *= 2;
				buf = realloc(buf, bufsize);
				
			}
			buf[pos++] = c;
		}
		else {
			buf[pos] = '\0'; // null byte
			printf("buffer: %s\n", buf);
			return buf;
		}
	}
}
// char *inputLine(FILE* fp, size_t size){
//     char *str;
//     int ch;
//     size_t len = 0;
//     str = realloc(NULL, sizeof(*str)*size);

//     if (!str) return str;

//     while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
//         str[len++]=ch;
//         if(len==size){
//             str = realloc(str, sizeof(*str)*(size+=16));
//             if(!str)return str;
//         }
//     }
//     str[len++]= '\0';

//     return realloc(str, sizeof(*str)*len);
// }

//Basically the same idea as inputLine function, dynamically allocate memory for the size.
char ** parseLine(char* line){
	int bufsize = 16;
	int position = 0;
	char **params = malloc(bufsize * sizeof(char*));
	char* param;

	param = strtok(line, " ");
	while (param != NULL) {
		params[position] = param;
		position++;

		if (position >= bufsize) {
			bufsize *= 2;
			params = realloc(params, bufsize * sizeof(char*));
    	}

    param = strtok(NULL, " ");
  }
  params[position] = '\0';
  return params;

}


int main(){ 
	while(1){
		//https://www.delftstack.com/howto/c/get-current-directory-in-c/
		//get the current working directory first
		char *path = getcwd(NULL, 0);
		//check if the path exists
		if (path == NULL) {
			perror("getcwd");
			exit(EXIT_FAILURE);
		}			
		//get current working directory path
		printf("hacker1@hacker:%s$ ", path);

		char* user_input;
		user_input = read_command_naive();
   	    char** tokens = parseLine(user_input);

		for(int i = 0; tokens[i] != NULL; i++){
   			printf("%s ", tokens[i]);
		}	

		char * command = tokens[0];

		if (strcmp (command, "exit") == 0){
			exit(0);
		} 
		else if (strcmp (command, "cd") == 0){
			chdir(tokens[1]);
			
		}
		else{
			//https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/waitpid.htm				
			pid_t childID = fork();
			pid_t endID;
			if(childID != 0){
				printf("PARENT\n");
				int status;
				endID = waitpid(childID, &status, 0);
				if (endID == -1) {
					perror("waitpid");
					exit(EXIT_FAILURE);
				}
				if (WIFEXITED(status)) {
					printf("Child process %d exited normally with status %d\n", endID, WEXITSTATUS(status));
				} else if (WIFSIGNALED(status)) {
					//printf("Child process %d terminated by signal %d\n", endID, WTERMSIG(status));
				}
				
			}else{
				 printf("CHILD\n");
				 //execvp returns -1 if the command is not executable
				 if (execvp(tokens[0], tokens) == -1){
					printf("Invalid Command\n");
				 }
			}
		}

		//Freeing this because malloc/realloc were called in the functions used by them.
		free(user_input);
		free(tokens);
		free(path);
	}

	return 0;
}


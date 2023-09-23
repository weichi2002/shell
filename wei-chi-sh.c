#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define FALSE 0
#define TRUE 1


//reads a line of input from a file, dynamically allocates memory for it, and adjusts the allocated size as needed. 
char *readCommand(void){
	int bufsize = 4;
	char *buf = malloc(sizeof(char) * bufsize);
	int pos = 0;

	while(1){
		char c = getchar();
		if(c !='\n'){
			//Need to allocate more space, doubling the memory every time as needed.
			if(pos >= bufsize){
				bufsize *= 2;
				buf = realloc(buf, bufsize);
				
			}
			buf[pos++] = c;
		}
		else {
			buf[pos] = '\0'; // null byte
			// printf("buffer: %s\n", buf);
			return buf;
		}
	}
}

//Basically the same idea as readCommand function, dynamically allocate memory for the size.
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
			//doubling the size as needed
			bufsize *= 2;
			params = realloc(params, bufsize * sizeof(char*));
    	}

    param = strtok(NULL, " ");
  }
  params[position] = '\0';
  return params;

}


int checkValidInput(const char* input){
	while (*input) {
        if (*input != ' ') {
            return TRUE; // The string is nonempty
        }
        input++;
    }
    return FALSE; // The string is empty
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

		char* userInput;
		userInput = readCommand();

		//validate the input so that it does not crash when user enters nothing
		if(checkValidInput(userInput) == FALSE)
			continue;

   	    char** tokens = parseLine(userInput);

		if (strcmp (tokens[0], "exit") == 0){
			exit(0);
		} 
		else if (strcmp (tokens[0], "cd") == 0){
			chdir(tokens[1]);
		}
		else if(strcmp (tokens[0], "echo") ==0){
			for(int i = 1; tokens[i] != NULL; i++)
				printf("%s ", tokens[i]);
			printf("\n");
		}
		else{
			//https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/waitpid.html				
			pid_t childID = fork();
			pid_t endID;
			if(childID != 0){
				// printf("PARENT\n");
				int status;
				endID = waitpid(childID, &status, 0);
				if (endID == -1) {
					perror("waitpid");
					exit(EXIT_FAILURE);
				}
				if (WIFEXITED(status)) {
					// printf("Child process %d exited normally with status %d\n", endID, WEXITSTATUS(status));
				} else if (WIFSIGNALED(status)) {
					//printf("Child process %d terminated by signal %d\n", endID, WTERMSIG(status));
				}
				
			}else{
				// printf("CHILD\n");
				 //execvp returns -1 if the command is not valid
				 if (execvp(tokens[0], tokens) == -1){
					printf("Invalid Command\n");
					exit(EXIT_FAILURE);
				 }
			}
		}

		//Freeing this because malloc/realloc were called in the functions used by them.
		free(userInput);
		free(tokens);
		free(path);
	}

	return 0;
}


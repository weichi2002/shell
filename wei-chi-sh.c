#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


//reads a line of input from a file, dynamically allocates memory for it, and adjusts the allocated size as needed. It then returns a pointer to the resulting string.
//https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
char *inputLine(FILE* fp, size_t size){
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(*str)*size);

    if (!str) return str;

    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(*str)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]= '\0';

    return realloc(str, sizeof(*str)*len);
}

//Basically the same idea as inputLine function, dynamically allocate memory for the size.
char ** parseLine(char* line){
	int bufsize = 64;
	int position = 0;
	char **params = malloc(bufsize * sizeof(char*));
	char* param;

	param = strtok(line, " ");
	while (param != NULL) {
		params[position] = param;
		position++;

		if (position >= bufsize) {
		bufsize += 64;
		params = realloc(params, bufsize * sizeof(char*));
		
    }

    param = strtok(NULL, " ");
  }
  params[position] = NULL;
  return params;

}


int main(int argc, char *argv[]){ 
	while(1){
		//https://www.delftstack.com/howto/c/get-current-directory-in-c/
		//get the current working directory first
		char *buf = getcwd(NULL, 0);
		//check if the path exists
		if (buf == NULL) {
			perror("getcwd");
			exit(EXIT_FAILURE);
		}			
		//get current working directory path
		printf("hacker1@hacker:%s$\n", buf);

		char* user_input;
		user_input = inputLine(stdin, 10);
   	    char** tokens = parseLine(user_input);

		for(int i = 0; tokens[i] != NULL; i++){
   			printf("\n%s\n", tokens[i]);
		}	

		char * command = tokens[0];

		if (strcmp (command, "exit") == 0){
			exit(0);
		} 
		else if (strcmp (command, "cd") == 0){
			printf("Here\n");
			if(tokens[1] == NULL){
				printf("Invalid Command");
				continue;
			}
			printf("%s\n",tokens[1]);
			chdir(tokens[1]);
			
		}
		else{
			//https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/waitpid.htm				
			pid_t childID = fork();
			pid_t endID;
			if(childID != 0){
				int status;
				endID = waitpid(childID, &status, WNOHANG|WUNTRACED);

				if (endID == childID) {  /* child ended                 */
					if (WIFEXITED(status)){
						printf("child finished");
						continue;
					}
					else if (WIFSIGNALED(status)){
						printf("Child ended because of an uncaught signal.n");
					}
					else if (WIFSTOPPED(status))
						printf("Child process has stopped.n");
					exit(EXIT_SUCCESS);
				}
			}else{
				
			}
		}

		free(user_input);
		free(tokens);
	}

	return 0;
}



#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80

// parse user input
void parseInput(char* input, char** args, int* background) {
	int i = 0; 
	char* token = strtok(input, " \n");

	// split input into args 
	while (token != NULL) {
		args[i++] = token;
		token = strtok(NULL, " \n");
	}

	args[i] = NULL; //terminate the arg list 
	
	// check for &: background process 
	if (i > 0 && strcmp(args[i-1], "&") == 0) {
		*background = 1; 
		args[i-1] = NULL; //remove & from args 

	} else {
		*background = 0; 
	}
}

int main(void) {
	char input[MAX_LINE]; // input buffer 
	char* args[MAX_LINE/2 + 1]; 
	int background;
	pid_t pid; // process ID 
	
	while(1) {
		printf("nemo> ");
		fflush(stdout); 

		//get user's input 
		if (fgets(input, MAX_LINE, stdin) == NULL) {
			perror("fgets error"); 
			continue; 
		}

		// parse input into args and check for background process 
		parseInput(input, args, &background);

		// is cmd empty? 
		if (args[0] == NULL) {
			continue; 
		}

		//terminate shell 
		if (strcmp(args[0], "exit") == 0) {
			break;
		}

		//child process w fork 
		pid = fork(); 

		//if fork fails 
		if (pid < 0) {
			perror("Fork Failed"); 
			exit(1);
		}
		else if (pid == 0) { //child process
			//execute cmd w execvp
			if (execvp(args[0], args) == -1) {
				perror("execvp failed");
			}
			exit(1); // if execvp fails? will this cause us issues we will see 
		} 
		else { //parent process 
		      	if (!background) {
				//wait for child to fin if not a bkgrnd
				waitpid(pid, NULL, 0); 
			} else {
				//bkgrnd process, no wait 
				printf("[Process running in background, PID: %d]\n", pid);
			}
		}
	}
	return 0;
} 

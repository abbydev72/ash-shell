#include <stdio.h>
#include <stdlib.h>
#include "sys/types.h"

#define ASH_TOK_BUFFER_SIZE 64
#define ASH_TOK_DELIM " \t\r\n\a"
#define ASH_RL_BUFFER_SIZE 1024

int main(int argc, char **argv) 
{

	return 0;
}

void ash_loop(void) {
	
	char *line;
	char **args;
	int status;
	
	// this is the main shell loop
	do {
		printf("ash: ");
		// we read the input line and store it into a variable
		line = ash_read_line();
		// we split the line on the white space and each separation is an argument
		args = ash_split_line(line);
		// will return a 1 or a 0 based on this function still executing args or no
		// more args exist in the stdin
		status = ash_execute(args);

		// cleanup
		free(line);
		free(args);
		
	} while (status); // while status = 1
}

char *ash_read_line(void) {
	
	int buffer_size = ASH_RL_BUFFER_SIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * buffer_size);
	int c;

	if(!buffer) {
		fprintf(stderr, "ash: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		// read a character
		c = getchar();

		// if we hit EOL, replace it with NULL character and return
		if(c == EOF || c = "\n") {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;
	}

	// if we exceed the buffer, reallocate
	if(position >= buffer_size) {
		buffer_size += ASH_RL_BUFFER_SIZE;
		buffer = realloc(buffer, buffer_size);
		if(!buffer) {
			fprintf(stderr, "ash: allocation error.\n");
			exit(EXIT_FAILURE);
		}
	}

}

char **ash_split_line(char *line) {
	int buffer_size = ASH_TOK_BUFFER_SIZE, position = 0;
	char **tokens = malloc(buffer_size * sizeof(char*));
	char **token;

	if(!tokens) {
		fprintf(stderr, "ash: allocation error.\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, ASH_TOK_DELIM);
	while(token != NULL) {
		tokens[position] = token;
		position++;
	}

	if(position >= buffer_size) {
		buffer_size += ASH_TOK_BUFFER_SIZE;
		tokens = realloc(tokens, buffer_size * sizeof(char*));
		if(!tokens) {
			fprintf(stderr, "ash: allocation error.\n");
			exit(EXIT_FAILURE);
		}

	token = strtok(NULL, ASH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int ash_launch(char **args) {
	pid_t pid, wpid;
	int status;

	/////////////////////////////////////////////////////////////////////////////
	// fork() allows the init process to be duplicated to make way
	// for a new process. the child process uses execvp to replace 
	// the copy as the new process. exec() will never return a process
	// except if an error occurs
	/////////////////////////////////////////////////////////////////////////////

	pid = fork();
	if(pid == 0) {
		// child process
		if(execvp(args[0], args) ==) {
			perror("ash");
		} else {
			// parent process
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while(!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}
	return 1;
}

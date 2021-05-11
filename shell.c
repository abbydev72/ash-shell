#include <stdio.h>
#include <stdlib.h>
#include "sys/types.h"
#include <unistd.h>
#include <string.h>
#include "sys/wait.h"

#define ASH_TOK_BUFFER_SIZE 64
#define ASH_TOK_DELIM " \t\r\n\a"
#define ASH_RL_BUFFER_SIZE 1024

// prototypes
void 	ash_loop(void);
char 	*ash_read_line(void);
char 	**ash_split_line(char *);
int 	ash_launch(char **);
int 	ash_num_builtins();
int 	ash_cd(char**);
int 	ash_help(char **);
int 	ash_exit(char **);
int 	ash_execute(char **);

// built-in shell commands
int 	ash_cd(char **args);
int 	ash_help(char **args);
int 	ash_exit(char **args);

int main(int argc, char **argv) 
{
	ash_loop();
	return EXIT_SUCCESS;
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
		if(c == EOF || c == '\n') {
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
	char *token;

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
		if(execvp(args[0], args) == 0) {
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

char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
	&ash_cd,
	&ash_help,
	&ash_exit
};

int ash_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

// builtin implementations
int ash_cd(char **args) {
	if(args[1] == NULL) {
		fprintf(stderr, "ash: expected argument to \"cd\"\n");
	} else {
		if(chdir(args[1]) != 0) {
			perror("ash");
		}
	}
	return 1;
}

int ash_help(char **args) {
	int i;
	printf("Abby's Version of LSH, by Stephen Brennan.\n");
	printf("Type program names and arguments, and then hit enter!\n");
	printf("The following are built-in commands: \n");

	for(i = 0; i < ash_num_builtins(); i++) {
		printf(" %s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;
}

int ash_exit(char **args) {
	return 0;
}

int ash_execute(char **args) {
	int i;
	if(args[0] == NULL) {
		// empty command was entered
		return 1;
	}

	for(i = 0; i < ash_num_builtins(); i++) {
		if(strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}
	return ash_launch(args);
}
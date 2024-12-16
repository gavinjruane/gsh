#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "shell.h"
#include "builtin.h"

#define DEFAULT_CMD_SIZE 64

extern char *builtin_str[];
extern int (*builtin_func[])(char **);

void int_handler(int sig) {
	write(STDOUT_FILENO, "\n", 1);
	write(STDOUT_FILENO, " $ ", 3);
	return;
}

int main(int argc, char **argv) {
	printf("gsh 1.0\nuser %s\n\n", getlogin());
	char *cmd_line;
	char **cmd_list;
	int status;

	struct sigaction sa;
	sa.sa_handler = int_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if ( sigaction(SIGINT, &sa, NULL) == -1 ) {
		fprintf(stderr, "gsh: could not set signal handler.\n");
		return EXIT_FAILURE;
	}

	do {
		printf(" $ ");
		cmd_line = get_cmd();
		cmd_list = parse_cmd(cmd_line);
		status = exec_all(cmd_list);

		free(cmd_line);
		free(cmd_list);
	} while ( status );
	
	return EXIT_SUCCESS;
}

char *get_cmd() {
	char *line = NULL;
	size_t line_size = 0;

	if ( getline(&line, &line_size, stdin) == -1 ) {
		exit(EXIT_SUCCESS);
	}

	return line;
}

char **parse_cmd(char *line) {
	int cmd_size = DEFAULT_CMD_SIZE;
	int pos = 0;
	char **cmd_tokens = malloc(cmd_size * sizeof(char *));
	char *token = NULL;

	if ( cmd_tokens == NULL ) {
		fprintf(stderr, "gsh: cannot allocate memory\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, " \t\r\n\a");
	while ( token != NULL ) {
		cmd_tokens[pos] = token;
		pos++;

		if ( pos >= cmd_size ) {
			cmd_size += DEFAULT_CMD_SIZE;
			cmd_tokens = realloc(cmd_tokens, cmd_size * sizeof(char * ));
			if ( cmd_tokens == NULL ) {
				fprintf(stderr, "gsh: cannot reallocate memory\n");
				exit(EXIT_FAILURE);
			}
		}

		int len_token = strlen(token);
		if ( token[len_token + 1] == '\"' ) {
			token = strtok(NULL, "\"");
		} else if ( token[len_token + 1] == '\'' ) {
			token = strtok(NULL, "\'");
		} else {
			token = strtok(NULL, " \t\r\n\a"); // keep using same src string
		}
	}
	cmd_tokens[pos] = NULL; // null out last
	return cmd_tokens;
}

int exec_cmd(char **args) {
	pid_t pid = fork();
	if ( pid == 0 ) {
		if ( execvp(args[0], args) == -1 ) {
			perror("gsh");
		}
		_exit(EXIT_FAILURE);
		// child
	} else if ( pid > 0 ) {
		wait(NULL);
		// parent
	} else if ( pid < 0 ) {
		fprintf(stderr, "gsh: fork error");
		exit(EXIT_FAILURE);
	}

	return 1;
}

int exec_all(char **args) {
	if ( args[0] == NULL ) {
		return 1;
	}

	for ( int i = 0; i < get_num_builtins(); i++ ) {
		if ( strcmp(args[0], builtin_str[i]) == 0 ) {
			return (*builtin_func[i])(args);
		}
	}

	return exec_cmd(args);
}

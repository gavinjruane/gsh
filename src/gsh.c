#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "internal.h"
#include "shell.h"
#include "builtin.h"

#define DEFAULT_CMD_SIZE 64

extern char *builtin_str[];
extern int (*builtin_func[])(char **);

static const struct redirection empty = { 0 };
struct redirection redirect = { 0 };

int macro_d = 0;
Macro *macros;
int num_macros = 0;

void int_handler(int sig) {
	write(STDOUT_FILENO, "\n", 1);
	write(STDOUT_FILENO, " $ ", 3);
	return;
}

int main(int argc, char **argv) {
	if ( argc != 1 ) {
		fprintf(stderr, "gsh: unexpected argument \"%s\"\n", argv[1]);
		return EXIT_FAILURE;
	}

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

	int history_d = prep_history_file();
	macro_d = prep_macro_file();
	FILE *macro_f;
       	if ( (macro_f = fdopen(macro_d, "r+")) == NULL ) {
		fprintf(stderr, "gsh: could not open macro file for reading\n");
		return EXIT_FAILURE;
	}
	macros = load_macros(macro_f);

	do {
		printf(" $ ");
		cmd_line = get_cmd();
		store_cmd(cmd_line, history_d);
		cmd_list = parse_cmd(cmd_line);
		status = exec_all(cmd_list);

		free(cmd_line);
		free(cmd_list);
	} while ( status );

	close(history_d);
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
		if ( (token[0] == '%') && (token[strlen(token) - 1] == '%') ) {
			char *result = search_macros(token);
			if ( result == NULL ) {
				fprintf(stderr, "gsh: invalid macro expression\n");
				exit(EXIT_FAILURE);
			}
			cmd_tokens[pos] = result;
		} else {
			cmd_tokens[pos] = token;
		}
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
		if ( redirect.type != RE_NONE ) {
			if ( redirect.type == RE_STDIN ) {
				dup2(redirect.to_d, redirect.from_d);
			} else if ( redirect.type == RE_STDOUT ) {
				dup2(redirect.to_d, redirect.from_d);
			} else if ( redirect.type == RE_STDERR ) {
				dup2(redirect.to_d, redirect.from_d);
			} else {
				fprintf(stderr, "gsh: invalid redirection\n");
				_exit(EXIT_FAILURE);
			}
		}
		if ( execvp(args[0], args) == -1 ) {
			perror("gsh");
		}
		_exit(EXIT_FAILURE);
		// child
	} else if ( pid > 0 ) {
		int wstatus;
		wait(&wstatus);
		if ( WIFEXITED(wstatus) ) {
			int exit = 0;
			if ( (exit = WEXITSTATUS(wstatus)) != 0 ) {
				printf("exit status: %d\n", exit);
			}
		}
		// parent
	} else if ( pid < 0 ) {
		fprintf(stderr, "gsh: fork error");
		exit(EXIT_FAILURE);
	}

	redirect = empty;
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#include "shell.h"
#include "builtin.h"

#define DEFAULT_CMD_SIZE 64
#define DEFAULT_SHORTCUT_SIZE 1024

extern struct redirection redirect;

extern int macro_d;

char *builtin_str[] = {
	"cd",
	"help",
	"exit",
	"redirect",
	"macro",
	"pipe"
};

int (*builtin_func[])(char **) = {
	&gsh_cd,
	&gsh_help,
	&gsh_exit,
	&gsh_redirect,
	&gsh_macro,
	&gsh_pipe
};

int get_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

int gsh_cd(char **args) {
	if ( args[1] == NULL ) {
		// chdir(); // go to home dir
		fprintf(stderr, "gsh: cd: expected argument to \"cd\"\n");
	} else if ( chdir(args[1]) == -1 ) {
		perror("gsh");
		return -1;
	}
	return 1;
}

int gsh_help(char **args) {
	printf("gsh: help: not implemented\n");
	return 1;
}

int gsh_exit(char **args) {
/*	if ( !args[1] ) {
		return 0;
	} else {
		return atoi(args[1]);
	} */
	return 0;
}

int gsh_redirect(char **args) {
	// redirect <stdin/stdout/stderr> <file>
	if ( !args[1] || !args[2] ) {
		fprintf(stderr, "gsh: redirect: expected arguments to \"redirect\"\n");
		return 1;
	} else {
		int redir_d = open(args[2], O_WRONLY | O_CREAT, 0666); // might be wrong for stdin
		if ( redir_d == -1 ) {
			fprintf(stderr, "gsh: shortcut: could not open %s for redirection\n", args[2]);
			return 1;
		}
		redirect.to_d = redir_d;

		if ( strcmp(args[1], "stdin") == 0 ) {
			redirect.from_d = STDIN_FILENO;
			redirect.type = RE_STDIN;
		} else if ( strcmp(args[1], "stdout") == 0 ) {
			redirect.from_d = STDOUT_FILENO;
			redirect.type = RE_STDOUT;
		} else if ( strcmp(args[1], "stderr") == 0 ) {
			redirect.from_d = STDERR_FILENO;
			redirect.type = RE_STDERR;
		} else {
			fprintf(stderr, "gsh: shortcut: must specify stdin, stdout, or stderr\n");
		}
	}
	return 1;
}

int gsh_macro(char **args) {
	// macro <absolute path> <shortcut name> -> %<shortcut name>
	if ( !args[1] || !args[2] ) {	
		fprintf(stderr, "gsh: macro: expected arguments to \"macro\"\n");
		fprintf(stderr, "usage: macro <absolute path> <shortcut name>\n");
		return 1;
	} else {
		char macro_keyval[DEFAULT_SHORTCUT_SIZE];
		sprintf(macro_keyval, "%s:%s\n", args[2], args[1]);
		if ( write(macro_d, macro_keyval, strlen(macro_keyval) * sizeof(char)) == -1 ) {
			fprintf(stderr, "gsh: macro: write error\n");
			return 1;
		}
	}
	return 1;
}

int gsh_pipe(char **args) {
	if ( !args[1] ) {
		fprintf(stderr, "gsh: pipe: expected arguments to \"pipe\"\n");
		fprintf(stderr, "usage: pipe <program> <args> ...\n");
	} else {
		fprintf(stderr, "gsh: pipe: pipes not available.\n");
	}
	return 1;
}

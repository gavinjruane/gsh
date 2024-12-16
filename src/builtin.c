#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "shell.h"
#include "builtin.h"

#define DEFAULT_CMD_SIZE 64

char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[])(char **) = {
	&gsh_cd,
	&gsh_help,
	&gsh_exit
};

int get_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

int gsh_cd(char **args) {
	if ( args[1] == NULL ) {
		fprintf(stderr, "gsh: expected argument to \"cd\"\n");
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
	return 0;
}


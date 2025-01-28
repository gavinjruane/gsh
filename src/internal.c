#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "internal.h"

extern Macro *macros;
extern int num_macros;

int store_cmd(char *cmd_line, int history_d) {
	if ( write(history_d, cmd_line, strlen(cmd_line) * sizeof(char)) == -1 ) {
		return -1;
	}
	return 1;
}

// Returns a file descriptor to a history file
int prep_history_file() {
	int history_f = open(".gsh_history", O_RDWR | O_APPEND | O_CREAT, 0666);
	if ( history_f == -1 ) {
		fprintf(stderr, "gsh: could not create/open history file");
		return -1;
	}
	return history_f;
}

int prep_macro_file() {
	int macro_f = open(".gsh_macros", O_RDWR | O_APPEND | O_CREAT, 0666);
	if ( macro_f == -1 ) {
		fprintf(stderr, "gsh: could not create/open macros file");
		return -1;
	}
	return macro_f;
}

Macro *load_macros(FILE *macro_f) {
	Macro *macros = malloc(sizeof(Macro) * DEFAULT_NUM_MACROS);
	if ( macros == NULL ) {
		fprintf(stderr, "gsh: cannot allocate memory\n");
		return NULL;
	}

	char *buffer = NULL;
	size_t count = 0;
	int line = 0;
	int macro_list_size = DEFAULT_NUM_MACROS;

	while ( getline(&buffer, &count, macro_f) > 0 ) {
		char *buffer_cp = buffer;
		char *macro = strsep(&buffer_cp, ":");
		char *expression = strsep(&buffer_cp, ":");
		if ( expression[strlen(expression) - 1] == '\n' ) {
			expression[strlen(expression) - 1] = '\0';
		}
		Macro macro_tmp;

		strcpy(macro_tmp.macro, macro);
		strcpy(macro_tmp.expression, expression);

		macros[line] = macro_tmp; // need to store in temp struct first then copy it over
		num_macros++;

		line++;
		if ( line == macro_list_size ) {
			macro_list_size += DEFAULT_NUM_MACROS;
			macros = realloc(macros, macro_list_size);
			if ( macros == NULL ) {
				fprintf(stderr, "gsh: cannot reallocate memory");
				return NULL;
			}
		}
	}
	free(buffer);

	return macros;
}

char *search_macros(char *macro_query) {
	macro_query[strlen(macro_query) - 1] = '\0';
	macro_query = &macro_query[1];
	
	for ( int i = 0; i < num_macros; i++ ) {
		if ( strcmp(macro_query, macros[i].macro) == 0 ) {
			return macros[i].expression;
		}
	}

	return NULL;
}

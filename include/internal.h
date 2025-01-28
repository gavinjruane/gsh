#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define DEFAULT_MACRO_SIZE 128
#define DEFAULT_EXPRS_SIZE 512
#define DEFAULT_NUM_MACROS 32

typedef struct macro {
	char macro[DEFAULT_MACRO_SIZE];
	char expression[DEFAULT_EXPRS_SIZE];
} Macro;

int store_cmd(char *cmd_line, int history_d);
int prep_history_file();
int prep_macro_file();
Macro *load_macros(FILE *macro_f);
char *search_macros(char *macro_query);

#endif

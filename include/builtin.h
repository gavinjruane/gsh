#ifndef BUILTIN_H
#define BUILTIN_H

#include <fcntl.h>

enum REDIRECTS {
	RE_NONE,
	RE_STDIN,
	RE_STDOUT,
	RE_STDERR
};

struct redirection {
	enum REDIRECTS type;
	int to_d;
	int from_d;
};

int get_num_builtins();
int gsh_cd(char **args);
int gsh_help(char **args);
int gsh_exit(char **args);
int gsh_redirect(char **args);

#endif

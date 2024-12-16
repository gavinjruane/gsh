#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

void int_handler(int sig);
char *get_cmd();
char **parse_cmd(char *line);
int exec_cmd(char **args);
int exec_all(char **args);

#endif

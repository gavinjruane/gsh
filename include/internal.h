#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

int store_cmd(char *cmd_line, int history_d);
int prep_history_file();

#endif

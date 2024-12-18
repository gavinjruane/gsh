#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "internal.h"

int store_cmd(char *cmd_line, int history_d) {
	if ( write(history_d, cmd_line, strlen(cmd_line) * sizeof(char)) == -1 ) {
		return -1;
	}
	return 1;
}

// Returns a file descriptor to a history file
int prep_history_file() {
	int history_f = open("gsh_history", O_RDWR | O_APPEND | O_CREAT, 0666);
	if ( history_f == -1 ) {
		fprintf(stderr, "gsh: could not create/open history file");
		return -1;
	}
	return history_f;
}

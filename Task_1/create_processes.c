#include "errno.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "sys/types.h"

// Error list:
// -1 : missing argument 'n'
// -2 : too many arguments are provided
// -3 : the input string is not a number
// -4 : the input number is less than zero
// -5 : the input number is our of range (the range is [LONG_MIN; LONG_MAX])

int main(int argc, char **argv) {
	char *strptr = NULL, *endptr = NULL;
	// Check argument's amount
	if (argc < 2) {
		printf("Too less arguments\n");
		return -1;
	} else if (argc > 2) {
		printf("Too many arguments\n");
		return -2;
	}

	// Check the string
	long input = 0;
	strptr = argv[1];
	input = strtol(strptr, &endptr, 10);


	if (endptr == strptr || *endptr != '\0') {
		printf("Wront input string\n");
		return -3;
	}

	if (input <= 0) {
		printf("The number must be greater then 0\n");
		return -4;
	}

	if (errno == ERANGE && (input == LONG_MAX || input == LONG_MIN)) {
		printf("Out of range\n");
		return -5;
	}

	pid_t pid;
	pid_t w;

	for (int i = 0; i < input; i++) {
		pid = fork();

		if (pid == -1) {
			// Error, process wasn't created

			printf("Error! Process wasn'r created!");
			return -6;
		}

		if (pid == 0)
		{
			// This code is to be run by the child

			int ppid = getppid();
			pid = getpid();

			printf("Child number %d, ID - %d, parent is %d\n", i + 1, pid, ppid);

			return 0;
		} else {
			// We are in the parent's process

			int status = 0;
			pid_t wpid = 0;

			wpid = waitpid(pid, &status, 0);
		}
	}
	return 0;
}


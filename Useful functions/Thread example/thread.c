#include "errno.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "sys/types.h"
#include "pthread.h"

// Error list:
// -1 : missing argument 'n'
// -2 : too many arguments are provided
// -3 : the input string is not a number
// -4 : the input number is less than zero
// -5 : the input number is our of range (the range is [LONG_MIN; LONG_MAX])

void* run(void *arg);

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

	int n = 0;
	int ret = 0;
	pthread_t* ids;
	ids = (pthread_t*) calloc(sizeof(pthread_t), input);

	for (int i = 0; i < input; i++) {
		ret = pthread_create(&ids[i], NULL, run, (void*) &n);		

		if (ret == -1) {
			printf("Error creating new thread!\n");
			return -6;
		}
	}

	for (int i = 0; i < input; i++) {
		pthread_join(ids[i], NULL);
	}

	printf("The value is %d\n", n);
	free(ids);
}

void* run(void *arg) {
	int *_arg = (int*) arg;

	for (int i = 0; i < 10000; i++) {
		(*_arg)++;
	}

	return 0;
}





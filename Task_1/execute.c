#include "stdio.h"
#include "unistd.h"

int main(int argc, char **argv) {
	int ret = 0;

	if (argc < 2) {
		printf("Wrong input arguments!\n");
		return -1;
	}

	ret = execvp(argv[1], &argv[1]);

	if (ret == -1) {
		printf("Error! Execution problems!\n");
		return -1;
	}

	return 0;
}


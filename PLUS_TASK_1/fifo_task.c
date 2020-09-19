#include <stdio.h>

int main(int argc, char* argv[]) {
	switch (argc) {
	case 1:
		// No arguments, read from the fifo

		break;
	case 2:
		// Write into the pipe from the file

		break;
	default:
		// Too many arguments
		printf("Error: too many arguments!\n");
		return -1;
	}
	return 0;
}
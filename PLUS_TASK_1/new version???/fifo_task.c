#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 5
#define FIFO_NAME "fifo"
#define PERMISSIONS S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define DELAY sleep(2)

int main(int argc, char* argv[]) {
	int fifo_file = 0;
	int file = 0;
	size_t read_bytes = 0;
	char buf[BUF_SIZE] = {0};

	switch (argc) {
	case 1: ;
		// No arguments, read from the fifo
		if (access(FIFO_NAME, R_OK) != 0) { 
			return -1;
		} else {
			fifo_file = open(FIFO_NAME, O_RDONLY);
			chmod(FIFO_NAME, 0200);                                                                                                                                                                                                                            chmod(FIFO_NAME, 0000);
		}

		while ((read_bytes = read(fifo_file, &buf, BUF_SIZE)) == BUF_SIZE) {
			write(STDOUT_FILENO, &buf, BUF_SIZE);
			DELAY;
		}

		// If something is left
		if (read_bytes) {
			write(STDOUT_FILENO, &buf, read_bytes);
			DELAY;
		}

		// Return standart permissions
		chmod(FIFO_NAME, PERMISSIONS);

		close(fifo_file);
		// Remove fifo file
		remove(FIFO_NAME);
		
		return 0;
	case 2: ;
		// Write into the fifo from the file

		// If file exists, the other process is running
		if (access(FIFO_NAME, W_OK) == 0) {
			return -1;
		}

		// Create the fifo
		if (mkfifo(FIFO_NAME, PERMISSIONS) == -1) {
			// can't create the fifo!
			return -1;
		}

		file = open(argv[1], O_RDONLY); // Open the file
		fifo_file = open(FIFO_NAME, O_WRONLY); // Open the FIFO

		while ((read_bytes = read(file, &buf, BUF_SIZE)) == BUF_SIZE) {
			write(fifo_file, &buf, BUF_SIZE);
			DELAY;
		}

		// If something is left
		if (read_bytes) {
			write(fifo_file, &buf, read_bytes);
			DELAY;
		}

		close(fifo_file);
		close(file);

		return 0;
	default:
		// Too many arguments
		printf("Error: too many arguments!\n");
		return -1;
	}
}
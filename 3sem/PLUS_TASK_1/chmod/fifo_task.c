#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 5
#define FIFO_NAME "fifo"
#define CREATE_FLAG S_IRUSR | S_IWUSR | S_IRGRP
#define PERMISSIONS S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define DELAY sleep(2)

void writeIntoFifo(const char *file_name);
void readFromFifo();

int main(int argc, char* argv[]) {
	switch (argc) {
	case 1: ;
		readFromFifo();
		
		return 0;
	case 2: ;
		writeIntoFifo(argv[1]);

		return 0;
	default:
		// Too many arguments
		printf("Error: too many arguments!\n");
		return -1;
	}
}

void writeIntoFifo(const char *file_name) {
	int fifo_file = 0;
	int file = 0;
	size_t read_bytes = 0;
	char buf[BUF_SIZE] = {0};
	struct stat statbuf = {0};

	// Write into the fifo from the file
	// If file exists, the other process is running

	if (access(FIFO_NAME, W_OK) == 0) {
		if (access(FIFO_NAME, R_OK) == 0) {
			// The existing fifo has read/write permissions
			// Another process is running/last run was bad
			remove(FIFO_NAME);
		} else {
			printf("Writer: Other process is running.\n");
			exit(-1);
		}
	}

	// Create the fifo
	if (mkfifo(FIFO_NAME, CREATE_FLAG) == -1) {
		if (errno == EEXIST) {
			// Fifo exists and we have write permissions,
			// which means loader was ended incorrectly
			chmod(FIFO_NAME, PERMISSIONS);
		} else {
			printf("Writer: Can't create the fifo file.\n");
			exit(-1);
		}
	}

	file = open(file_name, O_RDONLY); // Open the file
	if (file == -1) {
		printf("Writer: Can't access the input file.\n");
		exit(-1);
	}

	fifo_file = open(FIFO_NAME, O_WRONLY); // Open the FIFO
	if (fifo_file == -1) {
		printf("Writer: Can't open the fifo file.\n");
		exit(-1);
	}

	while ((read_bytes = read(file, &buf, BUF_SIZE)) == BUF_SIZE) {
		write(fifo_file, &buf, BUF_SIZE);
		DELAY;
	}

	if (read_bytes == -1) {
		printf("Writer: Error reading from the input file.\n");
		exit(-1);
	}
	// If something is left
	if (read_bytes) {
		write(fifo_file, &buf, read_bytes);
		DELAY;
	}

	close(fifo_file);
	close(file);
}

void readFromFifo() {
	int fifo_file = 0;
	int file = 0;
	size_t read_bytes = 0;
	char buf[BUF_SIZE] = {0};

	// No arguments, read from the fifo
	if (access(FIFO_NAME, R_OK) != 0) { 
		printf("Reader: can't access the fifo file.\n");
		exit(-1);
	} else {
		fifo_file = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);

		// Write only
		chmod(FIFO_NAME, 0200);                                                                                                                                                                                                                            chmod(FIFO_NAME, 0000);
	}

	while ((read_bytes = read(fifo_file, &buf, BUF_SIZE)) == BUF_SIZE) {
		write(STDOUT_FILENO, &buf, BUF_SIZE);
		DELAY;
	}

	if (read_bytes == -1) {
		printf("Reader: Error reading from the fifo file.\n");
		exit(-1);
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
}
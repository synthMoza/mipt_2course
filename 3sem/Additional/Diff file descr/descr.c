#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int descrSame(int fd1, int fd2);

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Wring arguments!\n");
		exit(-1);
	}

	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_RDONLY);

	if (fd1 == -1) {
		printf("Can't open file1!\n");
		exit(-1);
	}

	if (fd2 == -1) {
		printf("Can't open file2!\n");
		exit(-1);
	}

	// printf("fd1 = %d\n", fd1);
	// printf("fd2 = %d\n", fd2);

	if (descrSame(fd1, fd2)) {
		printf("Same\n");
	} else {
		printf("Different\n");
	}

	return 0;
}

int descrSame(int fd1, int fd2) {
	struct stat statbuf1;
	struct stat statbuf2;

	if (fstat(fd1, &statbuf1) == -1) {
		printf("Can't get fd1 status!\n");
		return -1;
	}


	if (fstat(fd2, &statbuf2) == -1) {
		printf("Can't get fd2 status!\n");
		return -1;
	}

	return (statbuf1.st_ino == statbuf2.st_ino) && (statbuf1.st_dev == statbuf2.st_dev);
}
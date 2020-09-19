#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	pid_t cpid;
	int pipefd[2];
	char buf;	
	int file;

	if (argc != 2) {
		printf("Wrong input\n");
		return -1;
	}

	if (pipe(pipefd) == -1) {
		printf("Pipe error\n");
		return -2;
	}

	cpid = fork();

	if (cpid == -1) {
		printf("Fork error\n");
		return -3;
	}

	if (cpid == 0) {
		// Child process
		file = open(argv[1], O_RDONLY);
		pipefd[0] = file; 

		close(pipefd[1]); //close write
		while (read(pipefd[0], &buf, 1) > 0) // read each symbol
                	write(STDOUT_FILENO, &buf, 1); // write it into stdout
		close(pipefd[0]); //close write
	} else {
		// Parent process
		close(pipefd[0]);          // close read
        write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);          // reader sees EOF
        wait(NULL);                // wait for child
        return 0;
	}

	return 0;
}

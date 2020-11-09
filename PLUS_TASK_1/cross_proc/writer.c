#include "fifo_task.h"

int main(int argc, char *argv[]) {
	pid_t cur_pid = 0;
	int flags = 0;
	int file = 0;
	int cross_proc = 0;
	int target_fifo = 0;
	char cross_proc_s[CROSS_PROC_FLENGTH] = {0};
	char buf[BUF_SIZE] = {0};
	int mark = 0;

	int read_bytes = 0;

	if (argc != 2) {
		perror("");
		printf("Writer: wrong argument input!\n");
		exit(-1);
	}

	cur_pid = getpid();

	// Create and open target fifo in nonblocking mode
	tostring(cur_pid, cross_proc_s);
	if (mkfifo(cross_proc_s, PERMISSIONS) == -1) {
		perror("");
		printf("Writer: can't create target fifo!\n");
		exit(-1);
	}
	target_fifo = open(cross_proc_s, O_RDWR | O_NONBLOCK);

	// Create shared fifo
	if (mkfifo(CROSS_PROC, PERMISSIONS) == -1 && errno != EEXIST) {
		perror("");
		printf("Writer: can't create CROSS_PROC fifo!\n");
		exit(-1);
	}

	// Wait for the reader to join this writer by openning in block mode
	cross_proc = open(CROSS_PROC, O_WRONLY);

	// Open input file
	file = open(argv[1], O_RDONLY);
	if (file == -1) {
		printf("Writer: can't open the input file!\n");
		exit(-1);
	}

	// Write current process's pid into cross_proc fifo
	if (write(cross_proc, &cur_pid, sizeof(pid_t)) == -1) {
		perror("");
		printf("Writer: error writing pid into fifo\n");
		exit(-1);
	}

	flags = fcntl(target_fifo, F_GETFL, 0);
	flags = fcntl(target_fifo, F_SETFL, flags & ~O_NONBLOCK);

	// printf("CHECK\n");
	// fflush(stdout);

	// // Check the state of reader
	// read_bytes = read(target_fifo, &mark, sizeof(int));
	// if (read_bytes != sizeof(int)) {
	// 	perror("");
	// 	printf("Writer: can't get reader state mark\n");
	// 	exit(-1);
	// }

	// printf("CHECK\n");
	// fflush(stdout);

	// Write everything into fifo from the input file
	while ((read_bytes = read(file, &buf, BUF_SIZE)) == BUF_SIZE) {
		if (write(target_fifo, &buf, read_bytes) == -1) {
			perror("");
			printf("Writer: error writing data into fifo\n");
			exit(-1);
		}
	}

	if (read_bytes == -1) {
		perror("");
		printf("Writer: error reading from file!\n");
		exit(-1);
	}

	if (read_bytes) {
		write(target_fifo, &buf, read_bytes);
	}

	close(target_fifo);
	close(file);
	close(cross_proc);

	return 0;
}
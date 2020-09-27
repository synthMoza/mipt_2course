#include "fifo_task.h"

int main(int argc, char *argv[]) {
	pid_t cur_pid = 0;
	int file = 0;
	int cross_proc = 0;
	int target_fifo = 0;
	char cross_proc_s[CROSS_PROC_FLENGTH] = {0};
	char buf[BUF_SIZE] = {0};

	int read_bytes = 0;

	if (argc != 2) {
		printf("Writer: wrong argument input!\n");
		exit(-1);
	}

	cur_pid = getpid();

	// Create target fifo
	tostring(cur_pid, cross_proc_s);
	if (mkfifo(cross_proc_s, PERMISSIONS) == -1) {
		printf("Writer: can't create target fifo!\n");
		exit(-1);
	}

	// Open fifo for sending pid of process
	cross_proc = open(CROSS_PROC, O_WRONLY);
	if (cross_proc == -1) {
		// Fifo hasn't been created yet
		if (mkfifo(CROSS_PROC, PERMISSIONS) == -1) {
			printf("Writer: can't create CROSS_PROC fifo!\n");
			exit(-1);
		}
		cross_proc = open(CROSS_PROC, O_WRONLY);
	}

	// Write current process's pid into cross_proc fifo
	write(cross_proc, &cur_pid, sizeof(pid_t));

	close(cross_proc);

	target_fifo = open(cross_proc_s, O_WRONLY);
	if (target_fifo == -1) {
		printf("Writer: can't open target fifo!\n");
		exit(-1);
	}

	// Open input file
	file = open(argv[1], O_RDONLY);
	if (file == -1) {
		printf("Writer: can't open the input file!\n");
		exit(-1);
	}

	// Write everything into fifo from the input file
	while ((read_bytes = read(file, &buf, BUF_SIZE)) == BUF_SIZE) {
		write(target_fifo, &buf, read_bytes);
		DELAY;
	}

	if (read_bytes == -1) {
		printf("Writer: error reading from file!\n");
		exit(-1);
	}

	if (read_bytes) {
		write(target_fifo, &buf, read_bytes);
		DELAY;
	}

	close(target_fifo);
	close(file);
	return 0;
}

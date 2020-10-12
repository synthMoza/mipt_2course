#include "fifo_task.h"

int main(int argc, char *argv[]) {
	pid_t cur_pid = 0;
	int flags = 0;
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

	// Open input file
	file = open(argv[1], O_RDONLY);
	if (file == -1) {
		printf("Writer: can't open the input file!\n");
		exit(-1);
	}
	// Create and open target fifo in nonblocking mode
	tostring(cur_pid, cross_proc_s);
	if (mkfifo(cross_proc_s, PERMISSIONS) == -1) {
		printf("Writer: can't create target fifo!\n");
		exit(-1);
	}
	target_fifo = open(cross_proc_s, O_RDWR | O_NONBLOCK);

	// Create shared fifo
	if (mkfifo(CROSS_PROC, PERMISSIONS) == -1 && errno != EEXIST) {
		printf("Writer: can't create CROSS_PROC fifo!\n");
		exit(-1);
	}
	// Wait for the reader to join this writer by openning in block mode
	cross_proc = open(CROSS_PROC, O_WRONLY);

	// Write current process's pid into cross_proc fifo
	write(cross_proc, &cur_pid, sizeof(pid_t));

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
	close(cross_proc);

	//remove(cross_proc_s);

	return 0;
}
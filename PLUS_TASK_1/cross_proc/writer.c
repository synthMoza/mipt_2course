#include "fifo_task.h"

int main(int argc, char *argv[]) {
	int flags = 0;
	int file = 0;
	int cross_proc = 0;
	int target_fifo = 0;
	char cross_proc_s[CROSS_PROC_FLENGTH] = {0};
	char buf[BUF_SIZE] = {0};
	pid_t read_pid = 0;

	int read_bytes = 0;

	if (argc != 2) {
		perror("");
		printf("Writer: wrong argument input!\n");
		exit(-1);
	}

	// Create shared fifo
	if (mkfifo(CROSS_PROC, PERMISSIONS) == -1 && errno != EEXIST) {
		perror("");
		printf("Writer: can't create CROSS_PROC fifo!\n");
		exit(-1);
	}

	// Wait for the writer to join this reader by openning in block mode
	cross_proc = open(CROSS_PROC, O_RDONLY);

	// Read the target fifo's name from cross_proc fifo
	read_bytes = read(cross_proc, &read_pid, sizeof(pid_t));

	if (read_bytes == -1 || read_bytes == 0) {
		perror("");
		printf("Writer: error reading writer's pid!\n");
		exit(-1);
	}

	// Open target fifo
	tostring(read_pid, cross_proc_s);
	target_fifo = open(cross_proc_s, O_WRONLY | O_NONBLOCK);
	if (target_fifo == -1) {
		perror("");
		printf("Writer: can't open the target fifo!\n");
		exit(-1);
	}

	// Open input file
	file = open(argv[1], O_RDONLY);
	if (file == -1) {
		printf("Writer: can't open the input file!\n");
		exit(-1);
	}

	flags = fcntl(target_fifo, F_GETFL, 0);
	flags = fcntl(target_fifo, F_SETFL, flags & ~O_NONBLOCK);

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
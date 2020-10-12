#include "fifo_task.h"

void tostring(int data, char *string);

int main() {
	int read_bytes = 0;
	pid_t read_pid = 0;
	int spliced = 0;
	int slct = 0;
	int flags = 0;
	int cross_proc = 0;
	int target_fifo = 0;
	char buf[BUF_SIZE] = {0};
	char cross_proc_s[CROSS_PROC_FLENGTH] = {0};

	// select vars
	fd_set readfds;
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;

	// Open cross proc fifo in nonblocking mode
	cross_proc = open(CROSS_PROC, O_RDONLY | O_NONBLOCK);
	if (cross_proc == -1) {
		printf("Reader: can't open cross proc fifo!\n");
		exit(-1);
	}

	FD_ZERO(&readfds);
	FD_SET(cross_proc, &readfds);

	if (select(cross_proc + 1, &readfds, NULL, NULL, &timeout) != 1) {
		printf("Select error!\n");
		exit(-1);
	}
	// Read the target fifo's name from cross_proc fifo
	read_bytes = read(cross_proc, &read_pid, sizeof(pid_t));

	if (read_bytes == -1 || read_bytes == 0) {
		printf("Reader: error reading writer's pid!\n");
		exit(-1);
	}

	// Open target fifo
	tostring(read_pid, cross_proc_s);
	target_fifo = open(cross_proc_s, O_RDONLY | O_NONBLOCK);
	if (target_fifo == -1) {
		printf("Reader: can't open the target fifo!\n");
		exit(-1);
	}

	flags = fcntl(target_fifo, F_SETFL, O_RDONLY);

	while ((spliced = splice(target_fifo, NULL, STDOUT_FILENO, NULL, BUF_SIZE, SPLICE_F_MOVE)) == BUF_SIZE) ;

	if (spliced == -1) {
		printf("Splice error!\n");
		exit(-1);
	}

	close(target_fifo);
	close(cross_proc);

	remove(cross_proc_s);
	
	return 0;
}
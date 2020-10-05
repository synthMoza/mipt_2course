#include "fifo_task.h"

void tostring(int data, char *string);

int main() {
	int read_bytes = 0;
	pid_t read_pid = 0;
	int slct = 0;
	int cross_proc = 0;
	int target_fifo = 0;
	char buf[BUF_SIZE] = {0};
	char cross_proc_s[CROSS_PROC_FLENGTH] = {0};

	// select vars
	fd_set readfds;
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;

	// Open cross proc fifo
	cross_proc = open(CROSS_PROC, O_RDWR | O_NONBLOCK);
	if (cross_proc == -1) {
		printf("Reader: can't open cross proc fifo!\n");
		exit(-1);
	}

	FD_ZERO(&readfds);
	FD_SET(cross_proc, &readfds);
	slct = select(cross_proc + 1, &readfds, NULL, NULL, &timeout);

	if (slct == -1) {
		printf("Reader: select error!\n");
		exit(-1);
	}

	if (slct == 0) {
		printf("Reader: \'select\' didn't find any data!\n");
		exit(-1);
	}

	if (!FD_ISSET(cross_proc, &readfds)) {
		printf("Reader: can't find file in readfds!\n");
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
	target_fifo = open(cross_proc_s, O_RDWR | O_NONBLOCK);
	if (target_fifo == -1) {
		printf("Reader: can't open the target fifo!\n");
		exit(-1);
	}

	FD_ZERO(&readfds);
	FD_SET(target_fifo, &readfds);

	while (1) {
		timeout.tv_sec = TIMEOUT_SEC;
		timeout.tv_usec = 0;
		slct = select(target_fifo + 1, &readfds, NULL, NULL, &timeout);

		if (slct == -1) {
			printf("Reader: select error!\n");
			exit(-1);
		}

		if (slct == 0) {
			printf("Reader: \'select\' didn't find any data!\n");
			exit(-1);
		}

		if (!FD_ISSET(target_fifo, &readfds)) {
			printf("Reader: can't find file in readfds!\n");
			exit(-1);
		}

		read_bytes = read(target_fifo, &buf, BUF_SIZE);

		if (read_bytes == BUF_SIZE) {
			write(STDOUT_FILENO, &buf, read_bytes);
			DELAY;
		} else {
			break;
		}
	}

	if (read_bytes == -1) {
		printf("Reader: error reading from target fifo!\n");
		exit(-1);
	}

	if (read_bytes) {
		write(STDOUT_FILENO, &buf, read_bytes);
		DELAY;
	}

	close(target_fifo);
	close(cross_proc);

	remove(cross_proc_s);
	
	return 0;
}
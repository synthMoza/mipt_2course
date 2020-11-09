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
	int mark = MARK;

	// select vars
	fd_set readfds;
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;

	// Create cross_proc fifo (if it doesn't exist)
	if (mkfifo(CROSS_PROC, PERMISSIONS) == -1 && errno != EEXIST) {
		perror("");
		printf("Reader: can't create CROSS_PROC fifo!\n");
		exit(-1);
	}

	// Open cross proc fifo in nonblocking mode
	cross_proc = open(CROSS_PROC, O_RDONLY | O_NONBLOCK);
	if (cross_proc == -1) {
		printf("Reader: can't open cross proc fifo!\n");
		exit(-1);
	}

	sleep(5);

	FD_ZERO(&readfds);
	FD_SET(cross_proc, &readfds);

	if (select(cross_proc + 1, &readfds, NULL, NULL, &timeout) != 1) {
		perror("");
		printf("Select error!\n");
		exit(-1);
	}
	// Read the target fifo's name from cross_proc fifo
	read_bytes = read(cross_proc, &read_pid, sizeof(pid_t));

	if (read_bytes == -1 || read_bytes == 0) {
		perror("");
		printf("Reader: error reading writer's pid!\n");
		exit(-1);
	}

	// Open target fifo
	tostring(read_pid, cross_proc_s);
	target_fifo = open(cross_proc_s, O_RDWR | O_NONBLOCK);
	if (target_fifo == -1) {
		perror("");
		printf("Reader: can't open the target fifo!\n");
		exit(-1);
	}

	flags = fcntl(target_fifo, F_GETFL, 0);
	flags = fcntl(target_fifo, F_SETFL, flags & ~O_NONBLOCK);

	// Send reader state mark (pipe was opened for reading)
	// if (write(target_fifo, &mark, sizeof(int)) != sizeof(int)) {
	// 	perror("");
	// 	printf("Reader: can't send state mark\n");
	// 	exit(-1);
	// }

	while ((read_bytes = read(target_fifo, buf, BUF_SIZE)) == BUF_SIZE) {
		write(STDOUT_FILENO, buf, BUF_SIZE);
	}

	if (read_bytes == -1) {
		perror("");
		printf("Read error!\n");
		exit(-1);
	}

	if (read_bytes) {
		write(STDOUT_FILENO, buf, read_bytes);
	}

	close(target_fifo);
	close(cross_proc);

	remove(cross_proc_s);
	
	return 0;
}
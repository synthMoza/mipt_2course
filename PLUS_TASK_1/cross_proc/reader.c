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
	int cur_pid = 0;

	cur_pid = getpid();
	// Create and open target fifo in nonblocking mode
	tostring(cur_pid, cross_proc_s);
	if (mkfifo(cross_proc_s, PERMISSIONS) == -1) {
		perror("");
		printf("Writer: can't create target fifo!\n");
		exit(-1);
	}
	target_fifo = open(cross_proc_s, O_RDONLY | O_NONBLOCK);

	// Create shared fifo
	if (mkfifo(CROSS_PROC, PERMISSIONS) == -1 && errno != EEXIST) {
		perror("");
		printf("Reader: can't create CROSS_PROC fifo!\n");
		exit(-1);
	}

	// Wait for the writer to join this reader by openning in block mode
	cross_proc = open(CROSS_PROC, O_WRONLY);

	// Write current process's pid into cross_proc fifo
	if (write(cross_proc, &cur_pid, sizeof(pid_t)) == -1) {
		perror("");
		printf("Reader: error writing pid into fifo\n");
		exit(-1);
	}

	flags = fcntl(target_fifo, F_GETFL, 0);
	flags = fcntl(target_fifo, F_SETFL, flags & ~O_NONBLOCK);

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
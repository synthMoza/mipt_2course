#include "fifo_task.h"

void tostring(int data, char *string);

int main() {
	int read_bytes = 0;
	pid_t read_pid = 0;
	int cross_proc = 0;
	int target_fifo = 0;
	char buf[BUF_SIZE] = {0};
	char cross_proc_s[CROSS_PROC_FLENGTH] = {0};

	// Open cross proc fifo
	cross_proc = open(CROSS_PROC, O_RDONLY);
	if (cross_proc == -1) {
		printf("Reader: can't open cross proc fifo!\n");
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
	target_fifo = open(cross_proc_s, O_RDONLY);
	if (target_fifo == -1) {
		printf("Reader: can't open the target fifo!\n");
		exit(-1);
	}

	// Read from target fifo and writeinto stdout
	while ((read_bytes = read(target_fifo, &buf, BUF_SIZE)) == BUF_SIZE) {
		write(STDOUT_FILENO, &buf, read_bytes);
		DELAY;
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
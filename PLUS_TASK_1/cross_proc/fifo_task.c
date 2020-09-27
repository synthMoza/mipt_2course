#include "fifo_task.h"

void tostring(int data, char *string) {
	char *ptr = string;
	while (data > 0) {
		*ptr = data % 10 + '0';
		ptr++;
		data /= 10;
	}

	*ptr = '\0';
}
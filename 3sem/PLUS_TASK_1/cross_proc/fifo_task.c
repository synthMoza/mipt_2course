#include "fifo_task.h"

#define STR_BUFFER 256 

void tostring(int data, char *string) {
	int counter = 0;
	char buf[STR_BUFFER] = {0};
	char *ptr = buf;

	while (data > 0) {
		*ptr = data % 10 + '0';
		ptr++;
		counter++;
		data /= 10;
	}
	*ptr = '\0';
	
	ptr = string;
	for (int i = 0; i < counter; i++) {
		*ptr = buf[counter - i - 1];
		ptr++;
	}
}
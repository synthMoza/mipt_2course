CC = gcc
CFLAGS = -Wall
all:
	@ ${CC} writer.c fifo_task.c -o writer ${CFLAGS}
	@ ${CC} reader.c fifo_task.c -o reader ${CFLAGS}

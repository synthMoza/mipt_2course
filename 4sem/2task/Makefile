CC = gcc
CFLAGS = -Wall -g -MD 

all: integral

integral: integral.o main.o input.o
	$(CC) -o $@ $^ -lm -lpthread

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf integral *.o *.d

-include *.d
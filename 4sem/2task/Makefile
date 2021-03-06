CC = gcc
CFLAGS = -Wall -g -MD 

all: integral

integral: integral.o main.o
	$(CC) -o $@ $^ -lm

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf integral *.o *.d

-include *.d
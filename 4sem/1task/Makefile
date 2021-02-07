CC = gcc
CFLAGS = -Wall -g -MD
MC = valgrind
MCFLAGS = -s

all: exec

exec: main.o avltree.o
	$(CC) -o $@ $^

memcheck: exec
	$(MC) $(MCFLAGS) ./$<
clean:
	rm -rf exec *.d *.o

-include *.d
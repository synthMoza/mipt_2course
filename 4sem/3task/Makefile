CC = gcc
CFLAGS = -Wall -g -MD 

all: server client

client: client.o integral.o
	$(CC) -o $@ $^ -lm -pthread

server: server.o input.o integral.o
	$(CC) -o $@ $^ -lm -pthread

integral.o: Thread_Integrate/integral.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf server client *.o *.d

-include *.d
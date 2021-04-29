CC = gcc
CFLAGS = -Wall -g -MD 

all: server client

client: client.o
	$(CC) -o $@ $^ -lm -pthread

server: server.o
	$(CC) -o $@ $^ -lm -pthread

input.o: ../Thread_Integrate/input.c
	$(CC) -c -o $@ $< $(CFLAGS)

integral.o: ../Thread_Integrate/integral.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf server *.o *.d

-include *.d
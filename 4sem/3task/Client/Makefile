CC = gcc
CFLAGS = -Wall -g -MD

all: client

client: client.o main.o integral.o
	$(CC) -o $@ $^ -lm -pthread

integral.o: ../Thread_Integrate/integral.c
	$(CC) -c -o $@ $< $(CFLAGS) 

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf client *.o *.d

-include *.d
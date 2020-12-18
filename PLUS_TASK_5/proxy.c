#include "proxy.h"

// Usage: ./proxy <n> <file>

int main(int argc, char* argv[]) {
    int n = 0;
    int i = 0;
    pid_t cpid = 0;
    int ret = 0;
    int nfds = 0;
    struct pipes* pipes = NULL;

    if (argc != 3)
        ERROR("Too few arguments!\n");

    // Number of clients
    n = input(argc, argv);
    if (n <= 0 || n == 1)
        ERROR("Wrong input!\n");

    // Allocate pipes
    pipes = (struct pipes*) calloc(n, sizeof(struct pipes));
    if (pipes == NULL)
        ERROR("Can't allocate memory for pipes!\n");

    // Create all pipes and children
    for (i = 0; i < n; ++i) {
        if (i != n -1) {
            // Last child writes into STDOUT
            ret = pipe(pipes[i].fromChild);
            if (ret == -1)
                ERROR("Pipe error!\n");

            // Parent reads it non-block
            ret = fcntl(pipes[i].fromChild[PIPE_RD], F_SETFL, O_NONBLOCK | O_RDONLY);
            if (ret == -1)
                ERROR("Fcntl error!\n");

            // Max descriptor for select
            nfds = MAX(nfds, pipes[i].fromChild[PIPE_RD]);
            nfds = MAX(nfds, pipes[i].fromChild[PIPE_WR]);
        }

        if (i != 0) {
            // The first child's desctiptor is the file, not the pipe
            ret = pipe(pipes[i].toChild);
            if (ret == -1) 
                ERROR("Pipe error\n!");

            // Parent writes non-block
            ret = fcntl(pipes[i].toChild[PIPE_WR], F_SETFL, O_NONBLOCK | O_WRONLY);
            if (ret == -1)
                ERROR("Fcntl error!\n");
            
            // Max descriptor for select
            nfds = MAX(nfds, pipes[i].toChild[PIPE_RD]);
            nfds = MAX(nfds, pipes[i].toChild[PIPE_WR]);
        }

        // Create new child
        cpid = fork();

        if (cpid > 0) {
            // Parent process, close child side
            if (i != 0)
                close(pipes[i].toChild[PIPE_RD]);
            if (i != n - 1)
                close(pipes[i].fromChild[PIPE_WR]);
        } else if (cpid == 0) {
            // Child process, close parent side that has already been created
            for (int j = 0; j < i; ++j)
                close(pipes[j].toChild[PIPE_WR]); // fromChild[PIPE_RD] already closed

            break;
        } else
            ERROR("Fork error!\n");
    }

    if (cpid > 0) {
        proxyParent(nfds + 1, pipes, n);
    } else {
        proxyChild(argv[2], i, n, pipes + i);
    }

    return 0;
}

void proxyChild(const char* file_name, int i, int n, struct pipes* pipes) {
    assert(pipes);
    assert(file_name);

    size_t written = 0;
    size_t buff_size = 0;
    size_t readbytes = 0;
    struct circleBuf cBuf;

    if (i > 0 && i < n -1) {
        // Not the first and the last
        close(pipes->toChild[PIPE_WR]);
        close(pipes->fromChild[PIPE_RD]);
    }

    if (i == 0) {
        // The first child reads the file
        close(pipes->fromChild[PIPE_RD]);

        pipes->toChild[PIPE_RD] = open(file_name, O_RDONLY);
        if (pipes->toChild[PIPE_RD] == -1)
            ERROR("Can't open the file!\n");
    }

    if (i == n - 1) {
        // The last one writes into STDOUT_FILENO
        close(pipes->toChild[PIPE_WR]);

        pipes->fromChild[PIPE_WR] = STDOUT_FILENO;
    }

    buff_size = CHBUFSIZE;
    cBuf.buffer = (char*) calloc(buff_size, sizeof(char));
    if (cBuf.buffer == NULL)
        ERROR("Calloc error!\n");

    // Initialize circle buffer
    cBuf.readIndex = 0;
    cBuf.writeIndex = 0;
    cBuf.endIndex = buff_size;
    cBuf.empty = buff_size;
    cBuf.full = 0;

    do {
        if (cBuf.empty > 0) {
            // Can read data into buffer
            readbytes = read(pipes->toChild[PIPE_RD], cBuf.buffer + cBuf.writeIndex, cBuf.empty);
            if (readbytes == -1)
                ERROR("Read from file error!\n");
            
            if (cBuf.writeIndex + readbytes == cBuf.endIndex) {
                // The end of the buffer is reached
                cBuf.writeIndex = 0;
                cBuf.full += readbytes;
                cBuf.empty = cBuf.readIndex - cBuf.writeIndex;
            } else {
                if (cBuf.writeIndex >= cBuf.readIndex) {
                    // Move the buffer
                    cBuf.writeIndex += readbytes;
                    cBuf.empty -= readbytes;
                    cBuf.full += readbytes;
                } else {
                    // Don't move
                    cBuf.writeIndex += readbytes;
                    cBuf.empty -= readbytes;
                }
            }
        }

        written = write(pipes->fromChild[PIPE_WR], cBuf.buffer + cBuf.readIndex, cBuf.full);
        if (written == -1)
            ERROR("Error writing to pipe!\n");

        if (cBuf.readIndex + written == cBuf.endIndex) {
            // The end of the buffer is reached
            cBuf.readIndex = 0;
            cBuf.full = cBuf.writeIndex - cBuf.readIndex;
            cBuf.empty += written;
        } else {
            if (cBuf.readIndex > cBuf.writeIndex) {
                // Move the buffer
                cBuf.readIndex += written;
                cBuf.full -= written;
                cBuf.empty += written;
            } else {
                // Don't move it
                cBuf.readIndex += written;
                cBuf.full -= written;
            }
        }
    } while (cBuf.full != 0 || readbytes != 0);

    close(pipes->fromChild[PIPE_WR]);
    close(pipes->toChild[PIPE_RD]);
    free(cBuf.buffer);
}

void proxyParent(int nfds, struct pipes* pipes, int n) {
    assert(pipes);

    int firstIndex = 0;
    int readyFds = 0;
    size_t readbytes = 0;
    size_t written = 0;
    int ret = 0;

    struct connection* connection = NULL;
    fd_set readfds, writefds;
    
    // Create n - 1 connections (the last child outputs data)
    connection = (struct connection*) calloc(n - 1, sizeof(struct connection));
    if (connection == NULL)
        ERROR("Calloc error!\n");

    // Initialize all connections
    for (int i = 0; i < n - 1; ++i) {
        connection[i].toChild = pipes[i + 1].toChild[PIPE_WR];
        connection[i].fromChild = pipes[i].fromChild[PIPE_RD];

        connection[i].bufSize = pow(3, n - i) * PROXYBUF;
        connection[i].cBuf.buffer = (char *)calloc(connection[i].bufSize, sizeof(char));
        if (connection[i].cBuf.buffer == NULL)
            ERROR("Calloc error!\n");
        
        connection[i].cBuf.endIndex = connection[i].bufSize;
        connection[i].cBuf.writeIndex = 0;
        connection[i].cBuf.readIndex = 0;

        connection[i].cBuf.full = 0;
        connection[i].cBuf.empty = connection[i].bufSize;
    }

    // Tranfser data
    do {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        for (int i = firstIndex; i < n - 1; ++i) {
            // Check all connections
            if (connection[i].fromChild >= 0 && connection[i].cBuf.empty > 0)
                FD_SET(connection[i].fromChild, &readfds);
            if (connection[i].toChild >= 0 && connection[i].cBuf.full > 0)
                FD_SET(connection[i].toChild, &writefds);
        }

        readyFds = select(nfds, &readfds, &writefds, NULL, NULL);
        if (readyFds == -1 && errno != EINTR) // !!!
            ERROR("Select error!\n");

        for (int i = firstIndex; i < n -1; ++i) {
            if (FD_ISSET(connection[i].fromChild, &readfds)) {
                // If the FD is ready to be read from
                readbytes = read(connection[i].fromChild,
                     connection[i].cBuf.buffer + connection[i].cBuf.readIndex, 
                        connection[i].cBuf.empty * sizeof(char));

                if (readbytes == -1)
                    ERROR("Read error!\n");

                if (readbytes == 0) {
                    // Time to close the connection
                    close(connection[i].fromChild);
                    connection[i].fromChild = -1;
                }

                if (connection[i].cBuf.readIndex + readbytes == connection[i].cBuf.endIndex) {
                    connection[i].cBuf.readIndex = 0;
                    connection[i].cBuf.full += readbytes;
                    connection[i].cBuf.empty = connection[i].cBuf.writeIndex - connection[i].cBuf.readIndex;
                } else {
                    if (connection[i].cBuf.readIndex >= connection[i].cBuf.writeIndex) {
                        connection[i].cBuf.readIndex += readbytes;
                        connection[i].cBuf.full += readbytes;
                        connection[i].cBuf.empty -= readbytes;
                    } else {
                        connection[i].cBuf.readIndex += readbytes;
                        connection[i].cBuf.empty -= readbytes;
                    }
                }
            }

            if (FD_ISSET(connection[i].toChild, &writefds) && connection[i].cBuf.full > 0) {
                // If the FD is ready to be written in
                written = write(connection[i].toChild, 
                    connection[i].cBuf.buffer + connection[i].cBuf.writeIndex, connection[i].cBuf.full * sizeof(char));
                if (written == -1)
                    ERROR("Write error!\n");

                if (connection[i].cBuf.writeIndex + written == connection[i].cBuf.endIndex) {
                    connection[i].cBuf.writeIndex = 0;
                    connection[i].cBuf.empty += written;
                    connection[i].cBuf.full = connection[i].cBuf.readIndex - connection[i].cBuf.writeIndex;
                } else {
                    if (connection[i].cBuf.writeIndex >= connection[i].cBuf.readIndex) {
                        connection[i].cBuf.full -= written;
                        connection[i].cBuf.empty += written;
                        connection[i].cBuf.writeIndex += written;
                    } else {
                        connection[i].cBuf.full -= written;
                        connection[i].cBuf.writeIndex += written;
                    }
                }
            }

            if (connection[i].fromChild == -1 && connection[i].cBuf.full == 0 && connection[i].toChild != -1) {
                close(connection[i].toChild);
                connection[i].toChild = -1;

                firstIndex++;
                free(connection[i].cBuf.buffer);
            }
        }
    } while (firstIndex < n - 1);

    // Close remaining connections
    for (int i = 0; i < n - 1; ++i) {
        if (connection[i].toChild != -1)
            close(connection[i].toChild);
        if (connection[i].fromChild != -1)
            close(connection[i].fromChild);
    }

    // Wait for remaining children
    for (int i = 0; i < n-1; ++i) {
        ret = wait(NULL);
        if (ret == -1) {
            if (errno == ECHILD)
                break;
            else
                ERROR("Wait error!\n");
        }
    }
}
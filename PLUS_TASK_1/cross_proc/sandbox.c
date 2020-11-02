#include "fifo_task.h"

int main() {
    int fctl = 0;
    int file = 0;
    char buf[256] = {0};

    file = open(CROSS_PROC, O_RDONLY | O_NONBLOCK);
    if (file == -1) {
        printf("Eror openning the file!\n");
        exit(-1);
    }
    
    /*
    fctl = fcntl(file, F_GETFD);
    fctl &= ~O_NONBLOCK;
    fctl = fcntl(file, F_SETFL, fctl);
    */

    int read_bytes = read(file, buf, 5);
    printf("%d\n", read_bytes);

    close(file);
    return 0;
}
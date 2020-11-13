#include "semshm.h"

int main(int argc, char *argv[]) {
    int file = 0;
    int key = 0;
    int tmp = 0;
    int semid = 0;
    int shmid = 0;
    int readbytes = 0;
    char *shm = NULL;
    char buf[BUF_SIZE] = {0};
    int value = 0;

    if (argc != 2) {
        printf("Wrong input!\n");
        exit(-1);
    }

    file = open(argv[1], O_RDONLY);
    if (file == -1) {
        printf("Can't open the file!\n");
        exit(-1);
    }

    tmp = open(KEY_FILE, IPC_CREAT | 0666);
    key = ftok(KEY_FILE, ID);

    // INIT
    semid = semget(key, SEM_NUM, IPC_CREAT | 0666);
    semInit(semid, 1, 1);
    semInit(semid, 3, 1);

    shmid = shmget(key, BUF_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Can't open shared memory!\n");
        semRemove(semid);
        exit(-1);
    }

    shm = (char*) shmat(shmid, NULL, 0);
    if (shm == (char*) -1) {
        printf("Can't get the adress of shm!\n");
        semRemove(semid);
        exit(-1);
    }
    
    // If writer dies, release Sem(0)
    // Sem(0) + 1(SEM_UNDO) - 1 - atomically
    struct sembuf sembuf[2];
    sembuf[0].sem_flg = 0;
    sembuf[0].sem_num = 0;
    sembuf[0].sem_op = 1;

    sembuf[1].sem_flg = SEM_UNDO;
    sembuf[1].sem_num = 0;
    sembuf[1].sem_op = -1;

    if (semop(semid, sembuf, 2) == -1) {
        perror("");
        return -1;
    }

    // Writer is alive
    V(semid, 4, SEM_UNDO);

    // Unique writer
    P(semid, 3, SEM_UNDO);

    // First iteration
    readbytes = read(file, buf + 1, BUF_SIZE - 1);
    if (readbytes == -1) {
        printf("\nWriter: reading error!\n");
        exit(-1);
    }
    if (readbytes > 0) {
        buf[0] = (readbytes == BUF_SIZE - 1) ? 0 : readbytes;

        P(semid, 1, 0);

        memcpy(shm, buf, BUF_SIZE);
        memset(buf, 0, BUF_SIZE);

        V(semid, 0, 0);
    }

    while ((readbytes = read(file, buf + 1, BUF_SIZE - 1)) == (BUF_SIZE - 1)) {
        buf[0] = (readbytes == BUF_SIZE - 1) ? 0 : readbytes;

        P(semid, 1, 0);

        value = semctl(semid, 4, GETVAL);
        if (value != 2) {
            printf("Writer: reader is dead\n");
            shmRemove(shm);
            semRemove(semid);
            exit(-1);
        }

        DELAY;
        memcpy(shm, buf, BUF_SIZE);
        memset(buf, 0, BUF_SIZE);

        V(semid, 0, 0);
    }

    if (readbytes == -1) {
        printf("Writer: reading error!\n");
        exit(-1);
    }

    if (readbytes > 0) {
        buf[0] = readbytes;
        P(semid, 1, 0);

        memcpy(shm, buf, readbytes + 1);
        memset(buf, 0, BUF_SIZE);

        value = semctl(semid, 4, GETVAL);
        if (value != 2) {
            printf("Writer: reader is dead\n");
            shmRemove(shm);
            semRemove(semid);
            exit(-1);
        }

        V(semid, 0, 0);
    }
    
    V(semid, 3, SEM_UNDO);
    P(semid, 4, SEM_UNDO);

    shmRemove(shm);
    semRemove(semid);
    close(tmp);

    return 0;
}

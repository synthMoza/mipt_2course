#include "semshm.h"

int main(int argc, char *argv[])
{
    char sign = 0;
    int key = 0;
    int tmp = 0;
    int semid = 0;
    int shmid = 0;
    int readbytes = 0;
    char *shm = NULL;
    char buf[BUF_SIZE] = {0};

    // Create key file in order to 
    tmp = open(KEY_FILE, IPC_CREAT | 0666);
    key = ftok(KEY_FILE, ID);

    // Open the semaphores (and maybe initialize them)
    semid = semget(key, SEM_NUM, IPC_EXCL | IPC_CREAT | 0666);
    if (semid == -1) {
        if (errno == EEXIST) {
            semid = semget(key, SEM_NUM, 0666);
            struct semid_ds ds;

            // Semaphores have already been created by other process
            for (int i = 0; i < MAX_TRIES; i++) {
                printf("Reader: attempt %d, wait for semaphore initialization...\n", i + 1);
                if (semctl(semid, 0, IPC_STAT, &ds) == -1) {
                    printf("Reader: error with IPC_STAT\n");
                    exit(-1);
                }
                if (ds.sem_otime != 0) {
                    printf("Reader: sucessful attempt!\n");
                semid = semget(key, SEM_NUM, 0666);
                    if (semid == -1) {
                        printf("Reader: error openning semaphore!\n");
                        exit(-1);
                    }
                    break;
                }
                else {
                    printf("Reader: Attempt failed!\n");
                }

                sleep(TRY_TIME);
            }
        }
        else {
            printf("Can't create semaphores!\n");
            exit(-1);
        }
    }
    else {
        // Perform 'semop' to change the value of 'sem_otime'
        struct sembuf sembuf;
        sembuf.sem_flg = 0;
        sembuf.sem_num = 2;
        sembuf.sem_op = 0;

        semInit(semid, 0, 1);
        semInit(semid, 1, 1);
        semInit(semid, 2, 0);
        semInit(semid, 3, 1);
        semInit(semid, 4, 1);

        if (semop(semid, &sembuf, 1) == -1) {
            printf("Reader: error during initialization!\n");
            semRemove(semid);
            exit(-1);
        }
    }

    shmid = shmget(key, BUF_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Can't open shared memory!\n");
        semRemove(semid);
        exit(-1);
    }

    shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        printf("Can't get the adress of shm!\n");
        semRemove(semid);
        exit(-1);
    }

    P(semid, 4, SEM_UNDO);
    while (sign != 1) {
        P(semid, 2, 0);
        P(semid, 0, SEM_UNDO);
        sign = shm[0];
        write(STDOUT_FILENO, shm + 1, BUF_SIZE - 1);
        memset(shm, 0, BUF_SIZE);
        V(semid, 0, SEM_UNDO);
        V(semid, 1, 0);
    }
    V(semid, 4, SEM_UNDO);

    shmRemove(shm);
    semRemove(semid);
    close(tmp);
    return 0;
}
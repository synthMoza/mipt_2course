#include "semshm.h"

int main(int argc, char *argv[])
{
    char sign = 0;
    int key = 0;
    int tmp = 0;
    int semid = 0;
    int shmid = 0;
    char *shm = NULL;
    int value = 0;
    int cnt = 0;

    // Create key file in order to 
    tmp = open(KEY_FILE, IPC_CREAT | 0666);
    key = ftok(KEY_FILE, ID);

    // INIT
    semid = semget(key, SEM_NUM, IPC_CREAT | 0666);
    semInit(semid, 2, 1);

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

    // Reader is alive
    V(semid, 4, SEM_UNDO);

    // Unique reader
    P(semid, 2, SEM_UNDO);

    while (sign == 0) {
        P(semid, 0, 0);

        // If this is first iteration
        if (cnt == 0) {
            // If reader dies, release Sem(1)
            // Sem(1) + 1(SEM_UNDO) - 1 - atomically
            struct sembuf sembuf[2];
            sembuf[0].sem_flg = 0;
            sembuf[0].sem_num = 1;
            sembuf[0].sem_op = 1;

            sembuf[1].sem_flg = SEM_UNDO;
            sembuf[1].sem_num = 1;
            sembuf[1].sem_op = -1;

            if (semop(semid, sembuf, 2) == -1) {
                perror("");
                return -1;
            }

            cnt++;
        }

        sign = shm[0];
        value = semctl(semid, 4, GETVAL);
        if (value != 2 && sign == 0) {
            printf("\nReader: writer is dead\n");
            shmRemove(shm);
            semRemove(semid);
            exit(-1);
        }
        if (sign == 0) {
            write(STDOUT_FILENO, shm + 1, BUF_SIZE - 1);
            memset(shm, 0, BUF_SIZE);
        } else if (sign > 0) {
            write(STDOUT_FILENO, shm + 1, sign);
        } else {
            printf("Reader: unknown error!\n");
            exit(-1);
        }
        

        V(semid, 1, 0);
    }
    V(semid, 2, SEM_UNDO);
    P(semid, 4, SEM_UNDO);

    shmRemove(shm);
    semRemove(semid);
    close(tmp);
    return 0;
}

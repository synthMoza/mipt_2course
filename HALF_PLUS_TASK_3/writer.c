#include "semshm.h"

// One shared memory, two semaphores:
// 1) Controls the amount of processes ran
// 2) Controlls access to the shared memory by several processes

int main(int argc, char *argv[]) {
    int file = 0;
    int key = 0;
    int semid = 0;
    int shmid = 0;
    int readbytes = 0;
    char *shm = NULL;
    char buf[BUF_SIZE] = {0};

    struct sembuf sembuf;
    sembuf.sem_num = 0;
    sembuf.sem_op = 0;
    sembuf.sem_flg = 0;

    if (argc != 2) {
        printf("Wrong input!\n");
        exit(-1);
    }

    file = open(argv[1], O_RDONLY);
    if (file == -1) {
        printf("Can't open the file!\n");
        exit(-1);
    }

    key = ftok(argv[1], ID);
    semid = semget(key, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        printf("Can't create semaphores!\n");
        exit(-1);
    }
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Can't create shared memory!\n");
        exit(-1);
    }

    shm = (char*) shmat(shmid, NULL, 0);
    if (shm == (char*) -1) {
        printf("Can't get the adress of shm!\n");
        exit(-1);
    }
    
    while ((readbytes = read(file, buf, BUF_SIZE)) != -1) {
        shm =   
    }

    return 0;
}
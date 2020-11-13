#include "semshm.h"

int semInit(int semid, size_t sem_num, int value) {
    if (semctl(semid, sem_num, SETVAL, value) == -1)
        return -1;      

    return 0;
}

int P(int semid, size_t sem_num, int flags) {
    struct sembuf sembuf;
    sembuf.sem_flg = flags;
    sembuf.sem_num = sem_num;
    sembuf.sem_op = -1;

    // Wait for semaphore value to become '> 0', then subtract 1
    if (semop(semid, &sembuf, 1) == -1) {
        return -1;
    }

    return 0;
}

int V(int semid, size_t sem_num, int flags) {
    struct sembuf sembuf;
    sembuf.sem_flg = flags;
    sembuf.sem_num = sem_num;
    sembuf.sem_op = 1;

    // Just increment value by 1
    if (semop(semid, &sembuf, 1) == -1) {
        return -1;
    }

    return 0;
}

int semRemove(int semid) {
    if (semctl(semid, 0, IPC_RMID) == -1)
        return -1;
    
    return 0;
}

int shmRemove(const void *shmaddr) {
    if (shmdt(shmaddr) == -1) 
        return -1;
    
    return 0;
}
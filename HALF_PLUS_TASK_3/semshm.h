#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SEM_MAX_OPS 32
#define KEY_FILE "keyfile"
#define ID 5
#define SHM_SIZE 32
#define PERMISSIONS 0666

// List of semaphores:
// 0 - for proper reader/writer synchronization
// 1 - indicate writer(s)
// 2 - write into shared memory
// 3 - indicate reader(s)
// 4 - read from shared memory

enum semNames {
    SYNC_SEM, WRITERS_SEM, WRITE_TO_SEM, READERS_SEM, READ_FROM_SEM
};

struct sembuf sembuf[SEM_MAX_OPS];
int op_count = 0;

// Add operation to the semop buffer with the counter inside
#define semAddSet(sem_n, sem_o, sem_f)                  \
    do {                                                \
        sembuf[op_count].sem_num = sem_n;               \
        sembuf[op_count].sem_op = sem_o;                \
        sembuf[op_count].sem_flg = sem_f;               \
        op_count++;                                     \ 
    } while (0);                                        

// Do all operations in the semop buffer and set the counter to zero
#define semOperate(semid)                               \
    do {                                                \
        if (semop(semid, sembuf, op_count) == -1) {     \
            printf("Semop failure!\n");                 \
            exit(-1);                                   \
        }                                               \
        op_count = 0;                                   \
    } while (0);                                

// Initialize certain semaphore from the set
int semInit(int semid, int sem_num, int value) {
    return semctl(semid, sem_num, SETVAL, value);
}
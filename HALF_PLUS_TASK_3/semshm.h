#pragma once

#define ID 3
#define BUF_SIZE 20
#define MAX_TRIES 10
#define SEM_NUM 5
#define TRY_TIME 1

#define DELAY sleep(1)

#define KEY_FILE "/tmp/key_file"

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

int semInit(int semid, size_t sem_num, int value);
int P(int semid, size_t sem_num, int flags);
int V(int semid, size_t sem_num, int flags);
int semRemove(int semid);
int shmRemove(const void* shmaddr);
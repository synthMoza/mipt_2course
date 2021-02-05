#include "semshm.h"

int main() {
    int semid = 0;
    key_t key = 0;
    int shmid = 0;
    int key_f = 0;
    char *shm = NULL;
    int size = SHM_SIZE;

    key_f = open(KEY_FILE, O_CREAT, PERMISSIONS);
    if (key_f == -1) {
        perror("");
        printf("Reader: can't access key file!\n");
        exit(-1);
    }

    key = ftok(KEY_FILE, ID);
    semid = semget(key, 5, IPC_CREAT | PERMISSIONS);
    if (semid == -1) {
        perror("");
        printf("Writer: can't get semaphores id!\n");
        exit(-1);
    }

    shmid = shmget(key, SHM_SIZE + sizeof(int), IPC_CREAT | PERMISSIONS);
    if (shmid == -1) {
        perror("");
        printf("Writer: can't get shared memory id!\n");
        exit(-1);
    }
    shm = (char*) shmat(shmid, NULL, 0);
    if (shm == (void*) -1) {
        perror("");
        printf("Writer: can't get shared memory!\n");
        exit(-1);
    }

    //========================================================

    // Indicate reader
    semAddSet(READERS_SEM, 0, 0);
    semAddSet(READERS_SEM, 1, SEM_UNDO);
    semOperate(semid);

    // Synchronization
    semAddSet(SYNC_SEM, 0, 0);
    semOperate(semid);

    // Initialize write to shared memory
    if (semInit(semid, WRITE_TO_SEM, 2) == -1) {
        perror("");
        printf("Reader: can't initialize semaphore!\n");
        exit(-1);
    }

    semAddSet(READERS_SEM, 1, SEM_UNDO);  // reader is ready now
    semAddSet(WRITE_TO_SEM, -1, SEM_UNDO); // undo if reader dies
    semOperate(semid);

    semAddSet(WRITERS_SEM, -2, 0);       // wait for reader
    semAddSet(WRITERS_SEM, 2, 0);        // return its state
    semAddSet(SYNC_SEM, 1, SEM_UNDO); // reader is alive
    semOperate(semid);

    //========================================================

    while (size == SHM_SIZE) {
        semAddSet(READ_FROM_SEM, -1, 0);
        semOperate(semid);

        if(semctl(semid, SYNC_SEM, GETVAL) != 2){
			// May be the last transfer
            if (*((int*) shm) == SHM_SIZE)
                break;
		}

        size = *((int *)shm);
        if (size != 0) {
            write(STDOUT_FILENO, shm + sizeof(int), size);
        }

        semAddSet(WRITE_TO_SEM, 1, 0);
        semOperate(semid);
    }

    return 0;
}
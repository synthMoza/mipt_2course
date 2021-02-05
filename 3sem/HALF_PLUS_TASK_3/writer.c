#include "semshm.h"

int main(int argc, char* argv[]) {
    int file = 0;
    int semid = 0;
    key_t key = 0;
    int key_f = 0;
    int shmid  = 0;
    char* shm = NULL;
    char buf[SHM_SIZE] = {0};
    int readbytes = 0;

    if (argc != 2) {
        printf("Writer: invalid number of arguments!\n");
        exit(-1);
    }

    key_f = open(KEY_FILE, O_CREAT, PERMISSIONS);
    if (key_f == -1) {
        perror("");
        printf("Writer: can't access key file!\n");
        exit(-1);
    }

    file = open(argv[1], O_RDONLY);
    if (file == -1) {
        printf("Writer: can't open the file!\n");
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
    //==============================================

    // Semaphore for indicating writer(s)
    semAddSet(WRITERS_SEM, 0, 0);
    semAddSet(WRITERS_SEM, 1, SEM_UNDO);
    semOperate(semid);

    // Semaphore for synchronization
    semAddSet(SYNC_SEM, 0, 0);
    semOperate(semid);

    // Initialize semaphore for reading from memory
    if (semInit(semid, READ_FROM_SEM, 1) == -1) {
        perror("");
        printf("Writer: can't initialize semaphore!\n");
        exit(-1);
    }

    semAddSet(WRITERS_SEM, 1, SEM_UNDO);  // writer's ready
    semAddSet(READ_FROM_SEM, -1, SEM_UNDO); // undo if writer dies
    semOperate(semid);

    semAddSet(READERS_SEM, -2, 0);       // wait for reader
    semAddSet(READERS_SEM, 2, 0);        // return its state
    semAddSet(SYNC_SEM, 1, SEM_UNDO); // indicate writer is alive
    semOperate(semid);

    //========================================================

    while ((readbytes = read(file, buf, SHM_SIZE)) > 0) {
        semAddSet(WRITE_TO_SEM, -1, 0);
        semOperate(semid);

        if(semctl(semid, SYNC_SEM, GETVAL) != 2) {
            break;
		}

        memcpy(shm + sizeof(int), buf, readbytes);
        *((int*) shm) = readbytes;

        semAddSet(READ_FROM_SEM, 1, 0);
        semOperate(semid);
    }

    if (readbytes == -1) {
        perror("");
        printf("Writer: read error!\n");
        exit(-1);
    }

    return 0;
}
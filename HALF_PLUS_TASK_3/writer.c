#include "semshm.h"

// List of semaphores:
// 0 - for proper reader/writer synchronization
// 1 - indicate writer(s)
// 2 - write into shared memory
// 3 - indicate reader(s)
// 4 - read from shared memory

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
    semAddSet(1, 0, 0);
    semAddSet(1, 1, SEM_UNDO);
    semOperate(semid);

    // Semaphore for synchronization
    semAddSet(0, 0, 0);
    semOperate(semid);

    // Initialize semaphore for reading from memory
    if (semInit(semid, 4, 1) == -1) {
        perror("");
        printf("Writer: can't initialize semaphore!\n");
        exit(-1);
    }

    semAddSet(1, 1, SEM_UNDO);  // writer's ready
    semAddSet(4, -1, SEM_UNDO); // undo if writer dies
    semOperate(semid);

    semAddSet(3, -2, 0);       // wait for reader
    semAddSet(3, 2, 0);        // return its state
    semAddSet(0, 1, SEM_UNDO); // indicate writer is alive
    semOperate(semid);

    //========================================================

    while ((readbytes = read(file, buf, SHM_SIZE)) > 0) {
        semAddSet(2, -1, 0);
        semOperate(semid);

        if(semctl(semid, 0, GETVAL) != 2) {
            break;
		}

        memcpy(shm + sizeof(int), buf, readbytes);
        *((int*) shm) = readbytes;

        semAddSet(4, 1, 0);
        semOperate(semid);
    }

    if (readbytes == -1) {
        perror("");
        printf("Writer: read error!\n");
        exit(-1);
    }

    return 0;
}
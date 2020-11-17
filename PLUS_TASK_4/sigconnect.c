#include "sigconnect.h"

int current = -1;

//! Get the exact bit from the symbol (counting from zero)
char get_bit(char a, char no_of_bit) {
    return (a & (1 << no_of_bit)) >> no_of_bit;
}

//! Parent's SIGUSR1 handler 
void sigusr1_p_handler(int signum) {
    current = 0;
}

//! Parent's SIGUSR2 handler 
void sigusr2_p_handler(int signum) {
    current = 1;
}

//! Parent's SIGCHLD handler
void sigchld_p_handler(int signum) {
    exit(-1);
}

//! Child's SIGUSR1 handler
void sigusr1_c_handler(int signum) {}

//! Child's SIGUSR2 handler
void sigusr2_c_handler(int signum) {}

//! Child's SIGHUP (parent's death) handler
void sighup_c_handler(int signum) {
    exit(-1);
}

int main(int argc, char* argv[]) {
    pid_t cpid;
    struct sigaction sigact;
    int sigactret = 0;
    int killret = 0;
    sigset_t sigset;
    sigset_t oldset;

    if (argc != 2) {
        printf("Wrong input arguments!\n");
        exit(-1);
    }

    // Set up parent's signal handlers
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sigusr1_p_handler;
    sigactret = sigaction(SIGUSR1, &sigact, NULL);
    if (sigactret == -1) {
        perror("sigaction");
        printf("Parrent: sigaction error!\n");
        exit(-1);
    }

    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sigusr2_p_handler;
    sigactret = sigaction(SIGUSR2, &sigact, NULL);
    if (sigactret == -1) {
        perror("sigaction");
        printf("Parrent: sigaction error!\n");
        exit(-1);
    }

    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sigchld_p_handler;
    sigactret = sigaction(SIGCHLD, &sigact, NULL);
    if (sigactret == -1) {
        perror("sigaction");
        printf("Parrent: sigaction error!\n");
        exit(-1);
    }

    // Set SIGUSR1/SIGUSR2 blocking mask (for borh parent and child)
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigprocmask(SIG_BLOCK, &sigset, &oldset);

    // Ready to fork
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(-1);
    }

    if (cpid == 0) {
        // Child process
        int file = 0;
        size_t readbytes = 0;
        char symbol = 0;
        int bit = 0;
        pid_t ppid = 0;
        
        // Prepare for parent's death
        prctl(PR_SET_PDEATHSIG, SIGHUP);

        // Set up child's signal handler's
        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = 0;
        sigact.sa_handler = sigusr1_c_handler;
        sigactret = sigaction(SIGUSR1, &sigact, NULL);
        if (sigactret == -1) {
            perror("sigaction");
            printf("Child: sigaction error!\n");
            exit(-1);
        }

        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = 0;
        sigact.sa_handler = sigusr2_c_handler;
        sigactret = sigaction(SIGUSR2, &sigact, NULL);
        if (sigactret == -1) {
            perror("sigaction");
            printf("Child: sigaction error!\n");
            exit(-1);
        }

        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = 0;
        sigact.sa_handler = sighup_c_handler;
        sigactret = sigaction(SIGHUP, &sigact, NULL);
        if (sigactret == -1) {
            perror("sigaction");
            printf("Child: sigaction error!\n");
            exit(-1);
        }

        ppid = getppid();

        file = open(argv[1], O_RDONLY);
        if (file == -1) {
           perror("open");
           printf("Child: error openning file!\n");
           exit(-1);
        }

        // Read from file by symbols
        while ((readbytes = read(file, &symbol, sizeof(char))) == sizeof(char)) {
            // Transfer this symbol by bits
            for (int i = sizeof(char) * 8 - 1; i >= 0; --i) {
                bit = get_bit(symbol, i);
                switch (bit) {
                    case 0:
                        // Send SIGUSR1
                        killret = kill(ppid, SIGUSR1);
                        if (killret == -1) {
                            if (errno != ESRCH) {
                                perror("kill");
                                printf("Child: kill error!\n");
                            }
                            exit(-1);
                        }
                        break;
                    case 1:
                        // Send SIGUSR2
                        killret = kill(ppid, SIGUSR2);
                        if (killret == -1) {
                            if (errno != ESRCH) {
                                perror("kill");
                                printf("Child: kill error!\n");
                            }
                            exit(-1);
                        }
                        break;
                    default:
                        printf("Child: unknown bit value %d!\n", bit);
                        exit(-1);
                }

                // Wait for SIGUSR1 from parent
                sigsuspend(&oldset);
            }
        }

        if (readbytes == -1) {
            perror("read");
            printf("Child: read error!\n");
            exit(-1);
        }

        close(file);
        return 0;
    } else {
        // Parent process
        int symbol = 0;
        int written = 0;
        sigset_t chldset;

        sigemptyset(&chldset);
        sigaddset(&chldset, SIGCHLD);

        while (1) {
            symbol = 0;

            for (int i = 0; i < sizeof(char) * 8; ++i) {
                // Wait for SIGUSR1/SIGUSR2
                sigsuspend(&oldset);
                symbol = (symbol << 1) | current;

                if (i == sizeof(char) * 8 - 1) {
                    // The last bit consumed, block SIGCHLD until succesful write
                    sigprocmask(SIG_BLOCK, &chldset, NULL);
                }

                killret = kill(cpid, SIGUSR1);
                if (killret == -1) {
                    if (errno != ESRCH) {
                        perror("kill");
                        printf("Parent: kill error!\n");
                    }
                    
                    exit(-1);
                }
            }

            written = write(STDOUT_FILENO, &symbol, sizeof(char));
            if (written == -1) {
                perror("write");
                printf("Parent: write error!\n");
                exit(-1);
            }

            sigprocmask(SIG_UNBLOCK, &chldset, NULL);
        }

        return 0;
    }

}

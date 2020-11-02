#include "msg.h"

int main(int argc, char *argv[]) {
	pid_t pid = 0;
	int ppid = 0;
	int i = 0;

	int msgid = 0;
	msgbuf msg;
	msg.mtype = 0;
	msg.num = 0;

	int n = input(argc, argv);
	if (n < 0) {
		return -1;
	}

	pid = getpid();
	ppid = getpid();
	msgid = msgget(pid, IPC_CREAT | 0666);
	if (msgid == -1) {
		printf("Can't create the message queue!\n");
		exit(-1);
	}

    msg.mtype = n;
    if (msgsnd(msgid, (void *)&msg, sizeof(msg.num), IPC_NOWAIT) == -1)
    {
        int errnum = errno;
        printf("Error is %d\n", errnum);
        printf("Error sending message %d!\n", i + 1);
        exit(-1);
    }

    for (i = 0; i < n; i++) {
		pid = fork();

		if (pid == 0)
			break;
	}

	if (pid == 0) {
		///! 000
			if (msgrcv(msgid, &msg, sizeof(msg.num), i + 1, 0) == -1) {
				int errnum = errno;
				printf("Error is %d\n", errnum);
				printf("Can't receive the message!\n");
				exit(-1);
			}

		printf("Parent - %d, me - %d, number - %d\n", ppid, getpid(), i + 1);
		fflush(stdout);

		//! 000
		
		msg.mtype = i;
		if (msgsnd(msgid, (void *) &msg, sizeof(msg.num), IPC_NOWAIT) == -1)
		{
			int errnum = errno;
			printf("Error is %d\n", errnum);
			printf("Error sending message %d!\n", i + 1);
			exit(-1);
		}
	} else {
		//! 000
		if (msgrcv(msgid, &msg, sizeof(msg.num), n, 0) == -1) {
			int errnum = errno;
			printf("Error is %d\n", errnum);
			printf("Can't receive the message!\n");
			exit(-1);
		}

		if (msgctl(msgid, IPC_RMID, NULL) == -1) {
			printf("Can't remove the queue!\n");
			exit(-1);
		}
	}

	return 0;
}
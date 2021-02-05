#pragma once

#include "stdio.h"
#include "limits.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "input.h"

#define ID 8

struct msgbuf {
	long mtype;
	int num;
};

typedef struct msgbuf msgbuf;
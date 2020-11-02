#pragma once

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 1024
#define CROSS_PROC_FLENGTH 256
#define CROSS_PROC "cross_proc"
#define PERMISSIONS S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define TIMEOUT_SEC 2
#define DELAY sleep(1)

void tostring(int data, char *string);

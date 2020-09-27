#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 5
#define CROSS_PROC_FLENGTH 256
#define CROSS_PROC "cross_proc"
#define PERMISSIONS S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define DELAY sleep(1)

void tostring(int data, char *string);
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

#define PATH_NAME "/tmp/server.shm"
#define PROJ_ID 123

#define SHMAT_FAILED (void*) -1

#ifndef _INFO_DEFINED_
#define _INFO_DEFINED_
struct info {
    pid_t pid;
    uid_t uid;
    gid_t gid;
    time_t elapsed;
    double load_1;
    double load_5;
    double load_15;
};
#endif
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

#define PATH_NAME "/tmp/lab5"
#define PROJ_ID 123

#define MMAP_FILE "/tmp/server.mmap"

#define SHMAT_FAILED (void*) -1

#define MSG_TYPE 1

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

#ifndef _MSG_BODY_DEFINED_
#define _MSG_BODY_DEFINED_
struct msg_body {
    int qid;
    struct info info;
};
#endif

#ifndef _MSG_DEFINED_
#define _MSG_DEFINED_
struct message {
    long type;
    struct msg_body body;
};
#endif
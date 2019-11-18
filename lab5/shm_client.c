#include "lib.h"
#include "client.h"
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {
    key_t key = ftok(PATH_NAME, PROJ_ID);
    int shmid = shmget(key, sizeof(struct info), 0644 | IPC_CREAT);
    if (shmid == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "shmget", strerror(errno));
        return EXIT_FAILURE;
    }

    struct info *info = (struct info*) shmat(shmid, NULL, 0);
    if (info == SHMAT_FAILED) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "shmat", strerror(errno));
        return EXIT_FAILURE;
    }

    print_info(info);
    return EXIT_SUCCESS;
}

#include "thread.h"
#include <sys/ipc.h>
#include <sys/sem.h>

#define NSEMAPHORES 3

#define INVERT_SEM 0
#define REVERSE_SEM 1
#define READY_SEM 2

static int semid;

void sig_handler(int sig) {
    printf("Releasing resources...\n");
    semctl(semid, 0, IPC_RMID);
    printf("All resources successfully released\n");
}

void* _invert_case() {
    struct sembuf lock = {
        .sem_num = INVERT_SEM,
        .sem_op = -1,
        .sem_flg = 0
    };
    struct sembuf unlock = {
        .sem_num = READY_SEM,
        .sem_op = 1,
        .sem_flg = 0
    };

    while (semop(semid, &lock, 1) != -1) {
        invert_case();
        semop(semid, &unlock, 1);
    }
    return NULL;
}

void* _reverse() {
    struct sembuf lock = {
        .sem_num = REVERSE_SEM,
        .sem_op = -1,
        .sem_flg = 0
    };
    struct sembuf unlock = {
        .sem_num = READY_SEM,
        .sem_op = 1,
        .sem_flg = 0
    };

    while (semop(semid, &lock, 1) != -1) {
        reverse();
        semop(semid, &unlock, 1);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);

    semid = semget(IPC_PRIVATE, NSEMAPHORES, 0644);
    if (semid == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "semget", strerror(errno));
        return EXIT_FAILURE;
    }

    pthread_t th_invert;
    if (pthread_create(&th_invert, NULL, _invert_case, NULL) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_create", strerror(errno));
        return EXIT_FAILURE;
    }

    pthread_t th_reverse;
    if (pthread_create(&th_reverse, NULL, _reverse, NULL) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_create", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sembuf unlock = {
        .sem_num = INVERT_SEM,
        .sem_op = 1,
        .sem_flg = 0
    };
    struct sembuf wait = {
        .sem_num = READY_SEM,
        .sem_op = -1,
        .sem_flg = 0
    };
    
    while (semop(semid, &unlock, 1) != -1) {
        if (semop(semid, &wait, 1) == -1) {
            break;
        }
        
        print_alphabet();
        if (unlock.sem_num == INVERT_SEM) {
            unlock.sem_num = REVERSE_SEM;
        } else {
            unlock.sem_num = INVERT_SEM;
        }

        sleep(1);
    }
    return EXIT_SUCCESS;
}
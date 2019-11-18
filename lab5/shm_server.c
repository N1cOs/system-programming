#include "lib.h"
#include "server.h"
#include <sys/ipc.h>
#include <sys/shm.h>

static int shmid;
static struct info *info;
static bool running = true;

void sig_handler(int sig) {
    printf("\n");
    print_dt_msg(stdout, "Releasing resources...");
    
    shmdt(info);
    shmctl(shmid, IPC_RMID, NULL);
    running = false;

    print_dt_msg(stdout, "All resources successfully released");
}

void update_info(struct info *info, time_t start_time) {
    double temp[3] = {0};
    int n = getloadavg(temp, 3);
    if (n == -1) {
        print_dt_msg(stderr, "Can't get info about average load");
    }
    
    info->elapsed = time(NULL) - start_time;
    info->load_1 = temp[0];
    info->load_5 = temp[1];
    info->load_15 = temp[2];
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);

    key_t key = ftok(PATH_NAME, PROJ_ID);
    shmid = shmget(key, sizeof(struct info), 0644 | IPC_CREAT);
    if (shmid == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "shmget", strerror(errno));
        return EXIT_FAILURE;
    }
    
    info = (struct info*) shmat(shmid, NULL, 0);
    if (info == SHMAT_FAILED) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "shmat", strerror(errno));
        return EXIT_FAILURE;
    }
    
    info->pid = getpid();
    info->uid = getuid();
    info->gid = getgid();
    time_t start_time = time(NULL);
    
    print_dt_msg(stdout, "Server started successfully");
    while(running) {
        update_info(info, start_time);
        sleep(1);
    }
    return EXIT_SUCCESS;
}
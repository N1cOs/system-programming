#include "server.h"
#include <sys/ipc.h>
#include <sys/msg.h>

static srv_qid;
static bool running = true;

void sig_handler(int sig) {
    print_dt_msg(stdout, "\nReleasing resources...");
    
    msgctl(srv_qid, IPC_RMID, NULL);
    running = false;
    
    print_dt_msg(stdout, "All resources successfully released");
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);

    key_t key = ftok(PATH_NAME, PROJ_ID);
    srv_qid = msgget(key, 0644 | IPC_CREAT);
    if (srv_qid == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "msgget", strerror(errno));
        return EXIT_FAILURE;
    }

    struct info srv_info = {
        .pid = getpid(),
        .uid = getuid(),
        .gid = getgid()
    };
    time_t start_time = time(NULL);
    update_info(&srv_info, start_time);

    print_dt_msg(stdout, "Server started successfully");
    while (running) {
        struct message msg;
        if (msgrcv(srv_qid, &msg, sizeof(struct message), MSG_TYPE, IPC_NOWAIT) == sizeof(struct message)) {
            print_dt_msg(stdout, "--> Received message");
            msg.body.info = srv_info;
            if (msgsnd(msg.body.qid, &msg, sizeof(struct message), 0) != -1) {
                print_dt_msg(stdout, "<-- Sent message");
            } else {
                print_dt_msg(stderr, strerror(errno));
            }
        } else {
            if (errno != ENOMSG) {
                print_dt_msg(stderr, strerror(errno));
            }
        }

        update_info(&srv_info, start_time);
        sleep(1);
    }

    return EXIT_SUCCESS;
}
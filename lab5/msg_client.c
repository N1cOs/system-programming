#include "client.h"
#include <sys/ipc.h>
#include <sys/msg.h>


int main(int argc, char *argv[]) {
    key_t srv_key = ftok(PATH_NAME, PROJ_ID);
    int srv_qid = msgget(srv_key, 0);
    if (srv_qid == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "msgget", strerror(errno));
        return EXIT_FAILURE;
    }

    int qid = msgget(IPC_PRIVATE, 0664);
    if (qid == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "msgget", strerror(errno));
        return EXIT_FAILURE;
    }

    struct message msg = {.type = MSG_TYPE, .body = {.qid = qid}};
    if (msgsnd(srv_qid, &msg, sizeof(struct message), 0) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "msgsnd", strerror(errno));
        msgctl(qid, IPC_RMID, NULL);
        return EXIT_FAILURE;
    }

    if (msgrcv(qid, &msg, sizeof(struct message), MSG_TYPE, 0) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "msgrcv", strerror(errno));
        msgctl(qid, IPC_RMID, NULL);
        return EXIT_FAILURE;
    }

    print_info(&msg.body.info);
    msgctl(qid, IPC_RMID, NULL);
    return EXIT_SUCCESS;
}
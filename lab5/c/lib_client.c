#include "client.h"

void print_info(struct info *info) {
    printf("server pid: %d\n", info->pid);
    printf("server uid: %d\n", info->uid);
    printf("server gid: %d\n", info->gid);
    printf("server up time: %lds\n", info->elapsed);
    printf("average load over the last 1 minute: %.2f%%\n", info->load_1);
    printf("average load over the last 5 minutes: %.2f%%\n", info->load_5);
    printf("average load over the last 15 minutes: %.2f%%\n", info->load_15);
}
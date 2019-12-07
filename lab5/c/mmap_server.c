#include "server.h"
#include <fcntl.h>
#include <sys/mman.h>

static int filefd;
static struct info *info;
static bool running = true;

void sig_handler(int sig) {
    printf("\nReleasing resources...\n");
    running = false;

    close(filefd);
    munmap(info, sizeof(struct info));
    unlink(MMAP_FILE);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);

    filefd = open(MMAP_FILE, O_CREAT | O_RDWR, 0644);
    if (filefd == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "open", strerror(errno));
        return EXIT_FAILURE;
    }
    
    if (ftruncate(filefd, sizeof(struct info)) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "frtuncate", strerror(errno));
        close(filefd);
        return EXIT_FAILURE;
    }

    info = mmap(NULL, sizeof(struct info), PROT_WRITE, MAP_SHARED, filefd, 0);
    if (info == MAP_FAILED) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "mmap", strerror(errno));
        close(filefd);
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
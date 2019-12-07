#include "client.h"
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
    int filefd = open(MMAP_FILE, O_RDONLY);
    if (filefd == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "open", strerror(errno));
        return EXIT_FAILURE;
    }

    struct info *info = mmap(NULL, sizeof(struct info), PROT_READ, MAP_SHARED, filefd, 0);
    if (info == MAP_FAILED) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "mmap", strerror(errno));
        close(filefd);
        return EXIT_FAILURE;
    }

    print_info(info);
    munmap(info, sizeof(struct info));
    close(filefd);
    
    return EXIT_SUCCESS;
}
#include "client.h"
#include <sys/socket.h>

#define SOCK_PATH "/tmp/smth"

int main(int argc, char *argv[]) {
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "socket", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr addr = {.sa_family = AF_UNIX, .sa_data = SOCK_PATH};
    if (connect(sockfd, &addr, sizeof(addr.sa_data)) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "connect", strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }

    struct info info;
    if (recv(sockfd, &info, sizeof(struct info), 0) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "recv", strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }

    print_info(&info);
    close(sockfd);

    return EXIT_SUCCESS;
}
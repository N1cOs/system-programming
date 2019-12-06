#include "server.h"
#include <fcntl.h>
#include <sys/socket.h>

#define SOCK_PATH "/tmp/smth"
#define QUEUE_SIZE 10

static int sock_srv;
static bool running = true;

void sig_handler(int sig) {
    printf("\nReleasing resources...\n");
    close(sock_srv);
    unlink(SOCK_PATH);
    running = false;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);

    sock_srv = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_srv == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "socket", strerror(errno));
        return EXIT_FAILURE;
    }

    int flags = fcntl(sock_srv, F_GETFL);
    if (flags == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "fcntl: getfl", strerror(errno));
        close(sock_srv);
        return EXIT_FAILURE;
    }
    if (fcntl(sock_srv, F_SETFL, flags | O_NONBLOCK) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "fcntl: setfl", strerror(errno));
        close(sock_srv);
        return EXIT_FAILURE;
    }

    struct sockaddr addr = {.sa_family = AF_UNIX, .sa_data = SOCK_PATH};
    if (bind(sock_srv, &addr, sizeof(addr.sa_data)) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "bind", strerror(errno));
        close(sock_srv);
        return EXIT_FAILURE;
    }

    if (listen(sock_srv, QUEUE_SIZE) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "listen", strerror(errno));
        close(sock_srv);
        unlink(SOCK_PATH);
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
        int sock_cli = accept(sock_srv, NULL, 0);
        if (sock_cli != -1) {
            print_dt_msg(stdout, "--> Accept connection");
            if (send(sock_cli, &srv_info, sizeof(struct info), 0) != -1) {
                print_dt_msg(stdout, "<-- Sent info to client");
            } else {
                print_dt_msg(stderr, strerror(errno));
            }
        } else {
            if (errno != EWOULDBLOCK) {
                 print_dt_msg(stderr, strerror(errno));
            }
        }

        update_info(&srv_info, start_time);
        sleep(1);
    }
    
    return EXIT_SUCCESS;
}
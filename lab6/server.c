#include "server.h"

static int srv_sock;
static bool running = true;

void sig_handler(int sig) {
    printf("\nGot %s signal\nReleasing resources...\n", strsignal(sig));
    close(srv_sock);
    running = false;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);

    int port = DEFAULT_PORT;
    struct hostent *hostent = gethostbyname(DEFAULT_HOST);
    if (hostent == NULL) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "gethostbyname", hstrerror(h_errno));
        return EXIT_FAILURE;
    }
    struct in_addr host = *((struct in_addr*) hostent->h_addr_list[0]);

    srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_sock == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "socket", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET, 
        .sin_port = htons(port),
        .sin_addr = host 
    };

    if (bind(srv_sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "bind", strerror(errno));
        close(srv_sock);
        return EXIT_FAILURE;
    }

    if (listen(srv_sock, MAX_CLIENTS) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "listen", strerror(errno));
        close(srv_sock);
        return EXIT_FAILURE;
    }

    print_dt_msg(stdout, "Server started\n");
    while (running) {
        int cli_sock = accept(srv_sock, NULL, NULL);
        if (cli_sock == -1) {
            if (running) {
                fprintf(stderr, "%s: %s: %s\n", argv[0], "accept", strerror(errno));
            }
            continue;
        }
        print_dt_msg(stdout, ACCEPT_MSG);
        
        int nulls = 0;
        size_t reqi = 0;
        size_t reqcap = BUFSIZ;
        char *reqbuf = malloc(sizeof(char) * reqcap);

        char buf[BUFSIZ];
        while (!(IS_END_OF_STREAM(nulls))) {
            ssize_t n = recv(cli_sock, buf, BUFSIZ, 0);
            for (ssize_t i = 0; i < n; i++) {
                if (reqi == reqcap) {
                    reqcap *= 2;
                    reqbuf = realloc(reqbuf, sizeof(char) * reqcap);
                }
                reqbuf[reqi++] = buf[i];

                if (buf[i] == '\0') {
                    nulls += 1;
                } else {
                    nulls = 0;
                }
            }
        }

        struct raw_request *raw_req = malloc(sizeof(struct raw_request));
        raw_req->cli_sock = cli_sock;
        raw_req->len = reqi - END_NULLS - 1;
        raw_req->req = reqbuf;

        pthread_t handle_thread;
        if (pthread_create(&handle_thread, NULL, handle, (void*)raw_req) != 0) {
           send_error(cli_sock, INTERNAL_ERROR);
           close(cli_sock);
           free(reqbuf);
           free(raw_req);
        }
    }

    return EXIT_SUCCESS;
}
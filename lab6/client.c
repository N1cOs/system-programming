#include "client.h"

int main(int argc, char *argv[]) {
    const struct option long_opts[] = {
        {"host", required_argument, 0, 'h'},
        {"base", required_argument, 0, 'b'},
        {0, 0, 0, 0}
    };
    const char *short_opts = "h:b:";

    int port = DEFAULT_PORT;
    struct in_addr host; 
    
    int opt;
    char *base = NULL;
    struct hostent *hostent = NULL;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (opt) {
            case 'h':
                hostent = gethostbyname(optarg);
                if (hostent == NULL) {
                    fprintf(stderr, "%s: %s: %s\n", argv[0], "gethostbyname", hstrerror(h_errno));
                    return EXIT_FAILURE;
                }
                host = *((struct in_addr*) hostent->h_addr_list[0]);
                break;
            case 'b':
                base = optarg;
                break;
            default:
                fprintf(stderr, "%s\n", USAGE);
                return EXIT_FAILURE;
                
        }
    }
    
    if (hostent == NULL) {
        host.s_addr = INADDR_LOOPBACK;
    }
    if (base == NULL) {
        fprintf(stderr, "%s: %s\n", argv[0], "specify the base directory");
        return EXIT_FAILURE;
    }
    if (argc - optind == 0) {
        fprintf(stderr, "%s: %s\n", argv[0], "specify at least one argument directory");
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "socket", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET, 
        .sin_port = htons(port),
        .sin_addr = host 
    };
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "connect", strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }

    struct request *req = new_request(BUFSIZ);
    req = wr_request(req, PROTOCOL_VERSION, strlen(PROTOCOL_VERSION) + 1);
    req = wr_request(req, SEP, SEP_LEN);
    req = wr_request(req, BASE_HEADER, strlen(BASE_HEADER));
    req = wr_request(req, base, strlen(base) + 1);
    req = wr_request(req, SEP, SEP_LEN);
    req = wr_request(req, SEP, SEP_LEN);
    for (int i = optind; i < argc; i++) {
        req = wr_request(req, argv[i], strlen(argv[i]) + 1);
        req = wr_request(req, SEP, SEP_LEN);
    }
    req = wr_request(req, END_OF_STREAM, END_NULLS);

    ssize_t n = 0;
    while (n != req->len) {
        ssize_t written = send(sockfd, req->body + n, req->len - n, 0);
        n += written;
    }

    size_t respptr = 0;
    size_t respcap = BUFSIZ;
    char *respbuf = malloc(sizeof(char) * respcap);

    char buf[BUFSIZ];
    while ((n = recv(sockfd, buf, BUFSIZ, 0)) != 0) {
        for (ssize_t i = 0; i < n; i++) {
            if (respptr == respcap) {
                respcap *= 2;
                respbuf = realloc(respbuf, sizeof(char) * respcap);
            }
            respbuf[respptr++] = buf[i];
        }
    }
    close(sockfd);

    struct response *resp = parse_response(respbuf, respptr - 1);
    if (resp->status == STATUS_OK) {
        for (int i = optind, j = 0; i < argc; i++, j++) {
            printf("%s:\n", argv[i]);
            struct dirinfo *dir = resp->body[j];
            for (int k = 0; k < dir->size; k++) {
                printf("\t%s\n", dir->files[k]);
            }
        }
    } else if (resp->status == STATUS_BAD) {
        printf("%s\n",(char*) resp->body);
    }

    

    return EXIT_SUCCESS;
}

struct request* wr_request(struct request *req, char *src, ssize_t len) {
    if (req->len + len > req->cap) {
        req->cap *= 2;
        req->body = realloc(req->body, req->cap);
    }
    
    memcpy(req->body + req->len, src, len);
    req->len += len;
    return req;
}

struct request* new_request(ssize_t capacity) {
    struct request *req = malloc(sizeof(struct request));
    req->len = 0;
    req->cap = capacity;
    req->body = malloc(capacity);
    return req;
}

struct response* parse_response(char *buf, size_t len) {
    struct response *resp = malloc(sizeof(struct response));
    
    size_t status_r = next_item(SEP, buf, 0, len);
    int status = buf[status_r - SEP_LEN];
    resp->status = status;
    
    size_t headers_r = next_item(SEP, buf, status_r + 1, len);
    if (status == STATUS_BAD) {
        size_t msg_r = next_item(SEP, buf, headers_r + 1, len);
        size_t msg_len = msg_r - headers_r - SEP_LEN;
        resp->body = malloc(sizeof(char*) * msg_len);
        memcpy(resp->body, buf + headers_r + 1, msg_len);
        return resp;
    }

    int dirs_size = 0;
    int dirs_cap = 10;
    struct dirinfo **dirs = malloc(sizeof(struct dirinfo*) * dirs_cap);

    size_t cur_dir_l = headers_r + 1;
    while (cur_dir_l < len) {
        size_t cur_dir_r = next_item(SEP, buf, cur_dir_l, len);

        int files_size = 0;
        int files_cap = 10;
        char **files = malloc(sizeof(char*) * files_cap);

        size_t cur_file_l = cur_dir_l;
        while (cur_file_l < cur_dir_r - 1) {
            size_t cur_file_r = next_item(FILE_SEP, buf, cur_file_l, len);
            
            size_t file_len = cur_file_r - cur_file_l;
            char *filename = malloc(sizeof(char) * file_len);
            strcpy(filename, buf + cur_file_l);

            if (files_size == files_cap) {
                files_cap *= 2;
                files = realloc(files, sizeof(char*) * files_cap);
            }

            files[files_size++] = filename;
            cur_file_l = cur_file_r + FILE_SEP_LEN;
        }

        if (dirs_size == dirs_cap) {
            dirs_cap *= 2;
            dirs = realloc(dirs, sizeof(struct dirinfo*) * dirs_cap);
        }
        
        struct dirinfo *dir = malloc(sizeof(struct dirinfo));
        dir->size = files_size;
        dir->files = files;

        dirs[dirs_size++] = dir;
        cur_dir_l = cur_dir_r + 1;
    }
    
    resp->status = status;
    resp->body = dirs;
    return resp;
}

size_t next_item(char *sep, char *buf, size_t start, size_t len) {
    size_t prev = start;
    for (size_t i = start + 1; i <= len; i++) {
        if (buf[prev] == sep[0] && buf[i] == sep[1]) {
            return i;
        }
        prev = i;
    }
    return PARSE_ITEM_ERROR;
}
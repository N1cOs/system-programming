#include "dirs_handle.h"

static void _send_dirinfo(int cli_sock, char status, struct dirinfo *dirs, int dirs_len);
static struct response* _write(struct response *resp, char *src, ssize_t len);

void* handle(void *rawreq_ptr) {
    struct raw_request *raw_req = (struct raw_request*) rawreq_ptr;
    struct request *req = parse_request(raw_req->req, raw_req->len);
    if (req == NULL) {
        send_error(raw_req->cli_sock, BAD_REQUEST);
        close(raw_req->cli_sock);
        print_dt_msg(stdout, RESPONSE_MSG);
        free(raw_req->req);
        free(raw_req);
        return NULL;
    }

    int curdir = 0;
    struct dirinfo *dirs = malloc(sizeof(struct dirinfo) * req->size);
    
    for (int i = 0; i < req->size; i++) {
        char *dirname = req->dirs[i];
        if (strncmp("/", dirname, 1) != 0) {
            dirname = malloc(strlen(req->base) + strlen(req->dirs[i]) + 1);
            strcpy(dirname, req->base);
            strcat(dirname, req->dirs[i]);
        }

        DIR *dir = opendir(dirname);
        if (dir == NULL) {
            send_error(raw_req->cli_sock, BAD_REQUEST);
            close(raw_req->cli_sock);
            print_dt_msg(stdout, RESPONSE_MSG);
            free(raw_req->req);
            free(raw_req);
            return NULL;
        }

        int files_len = 0;
        int files_cap = 10;
        char **files = malloc(sizeof(char*) * files_cap);

        struct dirent *dirent;
        while ((dirent = readdir(dir)) != NULL) {
            if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
                continue;
            }

            if (files_len == files_cap) {
                files_cap *= 2;
                files = realloc(files, sizeof(char*) * files_cap);
            }
            files[files_len++] = dirent->d_name;
        }

        struct dirinfo dinfo = {.n = files_len, .files = files};
        dirs[curdir++] = dinfo;
    }

    _send_dirinfo(raw_req->cli_sock, STATUS_OK, dirs, req->size);
    print_dt_msg(stdout, RESPONSE_MSG);

    for (int i = 0; i < req->size; i++) {
        free(dirs[i].files);
    }
    free(dirs);
    
    free(raw_req->req);
    free(raw_req);

    free(req->dirs);
    free(req->base);
    free(req);
    return NULL;
}

static void _send_dirinfo(int cli_sock, char status, struct dirinfo *dirs, int dirs_len) {
    struct response *resp = malloc(sizeof(struct response));
    char *body = malloc(BUFSIZ * sizeof(char));
    resp->len = 0;
    resp->cap = BUFSIZ;
    resp->body = body;

    resp = _write(resp, STATUS_HEADER, strlen(STATUS_HEADER));
    resp = _write(resp, &status, STATUS_LEN);
    resp = _write(resp, SEP, SEP_LEN);
    resp = _write(resp, SEP, SEP_LEN);

    for (int i = 0; i < dirs_len; i++) {
        char *file;
        struct dirinfo dir = dirs[i];
        for (int j = 0; j < dir.n - 1; j++) {
            file = dir.files[j];
            resp = _write(resp, file, strlen(file) + 1);
            resp = _write(resp, FILE_SEP, FILE_SEP_LEN);
        }
        if (dir.n > 0) {
            file = dir.files[dir.n - 1];
            resp = _write(resp, file, strlen(file) + 1);
        }
        resp = _write(resp, SEP, SEP_LEN);
    }

    ssize_t n = 0;
    while (n != resp->len) {
        ssize_t written = send(cli_sock, resp->body + n, resp->len - n, 0);
        n += written;
    }
    close(cli_sock);

    free(resp->body);
    free(resp);
}

static struct response* _write(struct response *resp, char *src, ssize_t len) {
    if (resp->len + len > resp->cap) {
        resp->cap *= 2;
        resp->body = realloc(resp->body, resp->cap);
    }
    
    memcpy(resp->body + resp->len, src, len);
    resp->len += len;
    return resp;
}

void send_error(int cli_sock, char *msg) {
    struct response *resp = malloc(sizeof(struct response));
    char *body = malloc(strlen(STATUS_HEADER) + STATUS_LEN + strlen(msg) + 1 + 3 * SEP_LEN);
    resp->len = 0;
    resp->cap = BUFSIZ;
    resp->body = body;
    
    char status = STATUS_BAD;
    resp = _write(resp, STATUS_HEADER, strlen(STATUS_HEADER));
    resp = _write(resp, &status, STATUS_LEN);
    resp = _write(resp, SEP, SEP_LEN);
    resp = _write(resp, SEP, SEP_LEN);
    resp = _write(resp, msg, strlen(msg) + 1);
    resp = _write(resp, SEP, SEP_LEN);

    send(cli_sock, resp->body, resp->len, 0);
    free(resp->body);
    free(resp);
}
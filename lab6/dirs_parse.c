#include "dirs_parse.h"

static size_t next_line(char buf[], size_t start, size_t len);

struct request* parse_request(char *buf, size_t len) {
    size_t version_r = next_line(buf, 0, len);
    if (version_r == PARSE_LINE_ERROR || strcmp(buf, PROTOCOL_VERSION) != 0) {
        return NULL;
    }

    size_t header_l = version_r + 1;
    size_t header_len = strlen(BASE_HEADER);
    ssize_t header_r = next_line(buf, header_l, len);
    if (header_r == PARSE_LINE_ERROR || strncmp(buf + header_l, BASE_HEADER, header_len) != 0) {
        return NULL;
    }
    
    char *base;
    size_t base_len = header_r - version_r - header_len - SEP_LEN;
    if (buf[header_r - SEP_LEN - 1] == '/') {
        base = malloc(sizeof(char) * base_len);
        strcpy(base, buf + header_l + header_len);
    } else {
        base = malloc(sizeof(char) * (base_len + 1));
        memcpy(base, buf + header_l + header_len, base_len - 1);
        base[base_len - 1] = '/';
        base[base_len] = '\0';
    }

    ssize_t headers_r = next_line(buf, header_r + 1, len);
    if (headers_r == PARSE_LINE_ERROR || headers_r != header_r + SEP_LEN) {
        return NULL;
    }

    int size = 0;
    int dirs_cap = 10;
    char **dirs = malloc(sizeof(char*) * dirs_cap);

    size_t cur_dir_l = headers_r + 1;
    size_t cur_dir_r;
    while (cur_dir_l < len) {
        if ((cur_dir_r = next_line(buf, cur_dir_l, len)) == PARSE_LINE_ERROR) {
            return NULL;
        }

        size_t dir_len = cur_dir_r - cur_dir_l - 1;
        char *dir = malloc(sizeof(char) * dir_len);
        strcpy(dir, buf + cur_dir_l);

        if (size == dirs_cap) {
            dirs_cap *= 2;
            dirs = realloc(dirs, sizeof(char*) * dirs_cap);
        }
        dirs[size++] = dir;
        
        cur_dir_l = cur_dir_r + 1;
    }

    struct request *req = malloc(sizeof(struct request));
    req->size = size;
    req->base = base;
    req->dirs = dirs;
    return req;
}

static size_t next_line(char buf[], size_t start, size_t len) {
    size_t prev = start;
    for (size_t i = start + 1; i <= len; i++) {
        if (IS_SEP(buf[prev], buf[i])) {
            return i;
        }
        prev = i;
    }
    return PARSE_LINE_ERROR;
}
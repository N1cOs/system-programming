#include <getopt.h>
#include "lib.h"
#include "dirs.h"

#define USAGE "Usage: client -h[--host] host DIR [DIR]..."

#define PARSE_ITEM_ERROR 0

#ifndef _REQUEST_DEFINED_
#define _REQUEST_DEFINED_
struct request {
    ssize_t len;
    ssize_t cap;
    char *body;
};
#endif

#ifndef _DIRINFO_DEFINED_
#define _DIRINFO_DEFINED_
struct dirinfo {
    int size;
    char **files;
};
#endif

#ifndef _RESPONSE_DEFINED_
#define _RESPONSE_DEFINED_
struct response {
    char status;
    struct dirinfo **body;
};
#endif

struct request* new_request(ssize_t capacity);
struct request* wr_request(struct request *req, char *src, ssize_t len);

size_t next_item(char *sep, char *buf, size_t start, size_t len);
struct response* parse_response(char *buf, size_t len);
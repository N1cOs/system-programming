#include "lib.h"
#include "dirs.h"

#define PARSE_LINE_ERROR 0

#define IS_SEP(prev, cur) prev == '\0' && cur == '/' 

#ifndef _REQUEST_DEFINED_
#define _REQUEST_DEFINED_
struct request {
    int size;
    char *base;
    char **dirs;
};
#endif

struct request* parse_request(char *buf, size_t len);
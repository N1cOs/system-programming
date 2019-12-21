#include <dirent.h>
#include "logs.h"
#include "dirs_parse.h"

#define RESPONSE_MSG "<-- Sent info to a client\n"

#ifndef _RAW_REQUEST_DEFINED_
#define _RAW_REQUEST_DEFINED_
struct raw_request {
    int cli_sock;
    ssize_t len;
    char *req;
};
#endif

#ifndef _DIRINFO_DEFINED_
#define _DIRINFO_DEFINED_
struct dirinfo {
    int n;
    char **files;
};
#endif

#ifndef _RESPONSE_DEFINED_
#define _RESPONSE_DEFINED_
struct response {
    ssize_t len;
    ssize_t cap;
    char *body;
};
#endif

void* handle(void *rawreq_ptr);
void send_error(int cli_sock, char *msg);
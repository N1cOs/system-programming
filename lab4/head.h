#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_NUMBER 10
#define USAGE_INFO "Usage: head [-c, --bytes=NUM] [-n, --lines=NUM] [FILE]..."
#define FILE_HEADER "==> %s <==\n"

#define ERR_BYTES "invalid number of bytes"
#define ERR_LINES "invalid number of lines"
#define ERR_FILE "cannot open '%s' for reading: %s\n"

uintmax_t parse_units(char *opt, bool count_lines);

int head_file(char *file_name, uintmax_t units, bool count_lines, bool print_header);
int head_lines(int fd, uintmax_t units, char *file_name);
int head_bytes(int fd, uintmax_t units, char *file_name);

#include "head.h"

uintmax_t parse_units(char *opt, bool count_lines) {
    uintmax_t units = strtoumax(opt, NULL, 10);
    const char *msg = count_lines ? ERR_LINES : ERR_BYTES;
    
    if (units == 0) {
        fprintf(stderr, "%s\n", msg);
        exit(EXIT_FAILURE);
    }
    if (units == UINTMAX_MAX) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return units;
}

int head_file(char *file_name, uintmax_t units, bool count_lines, bool print_header) {
    int fd;    
    if (*file_name == '-') {
        fd = STDIN_FILENO;
        file_name = "standart input";
    } else {
        fd = open(file_name, O_RDONLY);
    }

    if (fd < 0) {
        fprintf(stderr, ERR_FILE, file_name, strerror(errno));
        return EXIT_FAILURE;
    }
    if (print_header) {
        printf(FILE_HEADER, file_name);
    }

    int code = count_lines ? head_lines(fd, units, file_name) : 
                head_bytes(fd, units, file_name);
    close(fd);

    return code;
}

int head_bytes(int fd, uintmax_t units, char *file_name) {
    char buff[BUFSIZ];

    ssize_t n;
    while (units != 0 && (n = read(fd, buff, BUFSIZ)) != 0) {
        if (n < 0) {
            fprintf(stderr, ERR_FILE, file_name, strerror(errno));
            return EXIT_FAILURE;
        }

        for (int i = 0; i < n && units != 0; i++, units--) {
            printf("%c", buff[i]);
        }
    }

    return EXIT_SUCCESS;
}

int head_lines(int fd, uintmax_t units, char *file_name) {
    char buff[BUFSIZ];

    ssize_t n;
    while (units != 0 && (n = read(fd, buff, BUFSIZ)) != 0) {
        if (n < 0) {
            fprintf(stderr, ERR_FILE, file_name, strerror(errno));
            return EXIT_FAILURE;
        }

        for (int i = 0; i < n && units != 0; i++) {
            printf("%c", buff[i]);
            if (buff[i] == '\n') {
                units--;
            }
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    const struct option long_opts[] = {
        {"bytes", required_argument, 0, 'c'},
        {"lines", required_argument, 0, 'n'},
        {0, 0, 0, 0}
    };
    const char *short_opts = "c:n:";

    int opt;
    bool count_lines = true;
    uintmax_t units = DEFAULT_NUMBER;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (opt) {
            case 'c':
                count_lines = false;
                units = parse_units(optarg, count_lines);
                break;
            case 'n':
                count_lines = true;
                units = parse_units(optarg, count_lines);
                break;
            default:
                fprintf(stderr, "%s\n", USAGE_INFO);
                exit(EXIT_FAILURE);
        }
    }

    if (argc > optind) {
        if (argc - optind == 1) {
            return head_file(argv[optind], units, count_lines, false);
        }

        int code = 0;
        for (; optind != argc; optind++) {
            code |= head_file(argv[optind], units, count_lines, true);
            if (argc - optind != 1) {
                printf("\n");
            }
        }

        return code;
    }

    return head_file("-", units, count_lines, false);
}
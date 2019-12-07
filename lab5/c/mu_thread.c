#include "thread.h"
#include <getopt.h>

#define MILLISECOND 1000

static pthread_mutex_t mu;

void sig_handler(int sig) {
    printf("\nReleasing resources...\n");
    pthread_mutex_destroy(&mu);
}

int parse_int(char opt, char *num) {
    long res = strtol(num, NULL, 10);
    if (res == LONG_MIN || res <= 0) {
        fprintf(stderr, "Value for option `%c` should be greater than 0\n", opt);
        return -1;
    }
    
    if (res == LONG_MAX) {
        fprintf(stderr, "Value for option `%c` too big\n", opt);
        return -1;
    }
    return res;
}

void* _invert_case(void *_delay) {
    int delay = *((int*) _delay);
    
    usleep(delay);
    while (pthread_mutex_lock(&mu) == 0) {
        invert_case();
        pthread_mutex_unlock(&mu);
        usleep(delay);
    }
    return NULL;
}

void* _reverse(void *_delay) {
    int delay = *((int*) _delay);

    usleep(delay);
    while (pthread_mutex_lock(&mu) == 0) {
        reverse();
        pthread_mutex_unlock(&mu);
        usleep(delay);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);

    const struct option long_opts[] = {
        {"sleep-interval", required_argument, 0, 's'},
        {"invert-case-interval", required_argument, 0, 'i'},
        {"reverse-interval", required_argument, 0, 'r'},
        {0, 0, 0, 0}
    };
    const char *short_opts = "s:i:r:";

    int opt;
    int u_sleep = 500 * MILLISECOND, u_invert_case = 300 * MILLISECOND, u_reverse = 1000 * MILLISECOND;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (opt) {
            case 's':
                u_sleep = parse_int(opt, optarg);
                
                if (u_sleep == -1) {
                    return EXIT_FAILURE;
                }
                break;
            case 'i':
                u_invert_case = parse_int(opt, optarg);
                
                if (u_invert_case == -1) {
                    return EXIT_FAILURE;
                }
                break;
            case 'r':
                u_reverse = parse_int(opt, optarg);
                
                if (u_reverse == -1) {
                    return EXIT_FAILURE;
                }
                break;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                return EXIT_FAILURE;
        }
    }

    if (pthread_mutex_init(&mu, NULL) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_mutex_init", strerror(errno));
        return EXIT_FAILURE;
    }

    pthread_t invert_thread;
    if (pthread_create(&invert_thread, NULL, _invert_case, &u_invert_case) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_create", strerror(errno));
        return EXIT_FAILURE;
    }

    pthread_t reverse_thread;
    if (pthread_create(&reverse_thread, NULL, _reverse, &u_reverse) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_create", strerror(errno));
        return EXIT_FAILURE;
    }

    usleep(u_sleep);
    while (pthread_mutex_lock(&mu) == 0) {
        print_alphabet();
        pthread_mutex_unlock(&mu);
        usleep(u_sleep);
    }

    pthread_join(invert_thread, NULL);
    pthread_join(reverse_thread, NULL);
    return EXIT_SUCCESS;
}
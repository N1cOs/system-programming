#include "thread.h"
#include <getopt.h>

#define MILLISECOND 1000

static pthread_rwlock_t rwlock;

void sig_handler(int sig) {
    printf("\nReleasing resources...\n");
    pthread_rwlock_destroy(&rwlock);
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
    while (pthread_rwlock_wrlock(&rwlock) == 0) {
        invert_case();
        pthread_rwlock_unlock(&rwlock);
        usleep(delay);
    }
    return NULL;
}

void* _reverse(void *_delay) {
    int delay = *((int*) _delay);

    usleep(delay);
    while (pthread_rwlock_wrlock(&rwlock) == 0) {
        reverse();
        pthread_rwlock_unlock(&rwlock);
        usleep(delay);
    }
    return NULL;
}

void* _count_capitalized(void *_delay) {
    int delay = *((int*) _delay);

    usleep(delay);
    while (pthread_rwlock_rdlock(&rwlock) == 0) {
        int count = 0;
        for (size_t i = 0; i < ALPHABET_LEN; i++) {
            if (alphabet[i] >= 'A' && alphabet[i] <= 'Z') {
                count++;
            }
        }
        printf("Capitalized letters: %d\n", count);
        
        pthread_rwlock_unlock(&rwlock);
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
        {"count", required_argument, 0, 'c'},
        {0, 0, 0, 0}
    };
    const char *short_opts = "s:i:r:c:";

    int opt;
    int u_sleep = 500 * MILLISECOND, u_invert_case = 300 * MILLISECOND, 
            u_reverse = 1000 * MILLISECOND, u_count = 400 * MILLISECOND;
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
            case 'c':
                u_count = parse_int(opt, optarg);
                
                if (u_count == -1) {
                    return EXIT_FAILURE;
                }
                break;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                return EXIT_FAILURE;
        }
    }

    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_rwlocktex_init", strerror(errno));
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

    pthread_t count_thread;
    if (pthread_create(&count_thread, NULL, _count_capitalized, &u_count) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_create", strerror(errno));
        return EXIT_FAILURE;
    }

    usleep(u_sleep);
    while (pthread_rwlock_rdlock(&rwlock) == 0) {
        print_alphabet();
        pthread_rwlock_unlock(&rwlock);
        usleep(u_sleep);
    }

    pthread_join(invert_thread, NULL);
    pthread_join(reverse_thread, NULL);
    pthread_join(count_thread, NULL);
    return EXIT_SUCCESS;
}
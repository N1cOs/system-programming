#include "thread.h"
#include <semaphore.h>

sem_t invert_sem;
sem_t reverse_sem;
sem_t ready_sem;

void* _invert_case() {
    while (true) {
        sem_wait(&invert_sem);
        invert_case();
        sem_post(&ready_sem);
    }
}

void* _reverse() {
    while (true) {
        sem_wait(&reverse_sem);
        reverse();
        sem_post(&ready_sem);
    }
}


int main(int argc, char *argv[]) {
    if (sem_init(&invert_sem, 0, 0) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "sem_init", strerror(errno));
        return EXIT_FAILURE;
    }

    if (sem_init(&reverse_sem, 0, 0) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "sem_init", strerror(errno));
        return EXIT_FAILURE;
    }

    if (sem_init(&ready_sem, 0, 0) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "sem_init", strerror(errno));
        return EXIT_FAILURE;
    }

    pthread_t invert_thread;
    if (pthread_create(&invert_thread, NULL, _invert_case, NULL) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_create", strerror(errno));
        return EXIT_FAILURE;
    }

    pthread_t reverse_thread;
    if (pthread_create(&reverse_thread, NULL, _reverse, NULL) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pthread_create", strerror(errno));
        return EXIT_FAILURE;
    }

    sem_t *curr_sem = &invert_sem; 
    while (true) {
        sem_post(curr_sem);
        sem_wait(&ready_sem);
        
        print_alphabet();
        if (curr_sem == &invert_sem) {
            curr_sem = &reverse_sem;
        } else {
            curr_sem = &invert_sem;
        }

        sleep(1);
    }
}
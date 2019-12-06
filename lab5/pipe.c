#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define CMD "wc"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "%s: %s\n", argv[0], "specify one file as first argument");
        return EXIT_FAILURE;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "pipe", strerror(errno));
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], "fork", strerror(errno));
        return EXIT_FAILURE;
    }

    int status;
    if (pid == 0) {
        close(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            fprintf(stderr, "%s: %s: %s\n", argv[0], "dup2", strerror(errno));
            return EXIT_FAILURE;
        }

        char *cmd_argv[] = {CMD, NULL}; 
        if (execvp(CMD, cmd_argv) == -1) {
            fprintf(stderr, "%s: %s: %s\n", argv[0], "execvp", strerror(errno));
            return EXIT_FAILURE;
        }
    } else {
        close(pipefd[0]);

        int filefd = open(argv[1], O_RDONLY);
        if (filefd == -1) {
            fprintf(stderr, "%s: %s: %s\n", argv[0], "open", strerror(errno));
            return EXIT_FAILURE;
        }

        ssize_t n;
        char buf[BUFSIZ];
        while ((n = read(filefd, buf, BUFSIZ)) != 0) {
            if (n == -1) {
                close(filefd);
                close(pipefd[1]);
                fprintf(stderr, "%s: %s: %s\n", argv[0], "read", strerror(errno));
                return EXIT_FAILURE;
            }
            
            char res[n / 2];
            for (ssize_t i = 0; i < n; i++) {
                if ((i + 1) % 2 == 0) {
                    res[i / 2] = buf[i];
                }
            }
            
            if (write(pipefd[1], res, n / 2) == -1) {
                close(filefd);
                close(pipefd[1]);
                fprintf(stderr, "%s: %s: %s\n", argv[0], "write", strerror(errno));
                return EXIT_FAILURE;
            } 
        }
        close(pipefd[1]);
        close(filefd);
        wait(&status);
    }
    return status;
}
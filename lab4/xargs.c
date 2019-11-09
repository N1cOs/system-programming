#include "xargs.h"

int main(int argc, char *argv[]) {
    size_t arg_i = 0;
    size_t args_len = 100;
    char **stdin_args = malloc(sizeof(char*) * args_len);
    
    size_t line_i = 0;
    size_t line_buf_len = BUFSIZ;
    char *line_buff = malloc(sizeof(char) * line_buf_len);
    
    ssize_t n;
    char buff[BUFSIZ];
    while ((n = read(STDIN_FILENO, buff, BUFSIZ)) != 0) {
        if (n < 0) {
            perror("read");
            return EXIT_FAILURE;
        }

        for(ssize_t k = 0; k < n; k++) {
            if (IS_BLANCK(buff[k])) {
                if ((arg_i + 1) == args_len) {
                    args_len *= 2;
                    stdin_args = realloc(stdin_args, sizeof(char*) * args_len);
                    if (stdin_args == NULL) {
                        perror("realloc");
                        return EXIT_FAILURE;
                    }
                }
                stdin_args[arg_i] = malloc(sizeof(char*) * (line_i + 1));
                memcpy(stdin_args[arg_i], line_buff, line_i);
                *(stdin_args[arg_i] + line_i) = '\0';

                line_i = 0;
                arg_i++;
            } else {
                if ((line_i + 1) == line_buf_len) {
                    line_buf_len *= 2;
                    line_buff = realloc(line_buff, sizeof(char) * line_buf_len);
                    if (line_buff == NULL) {
                        perror("realloc");
                        return EXIT_FAILURE;
                    }
                }
                line_buff[line_i++] = buff[k];
            }
        }
    }
    free(line_buff);

    size_t cmd_args_len = (argc == 1 ? 3 : argc + 1);
    char ** cmd_args = malloc(sizeof(char*) * cmd_args_len);
    cmd_args[0] = (argc == 1 ? DEFAULT_CMD : argv[1]);
    for (int i = 2; i < argc; i++) {
        cmd_args[i - 1] = argv[i];
    }
    cmd_args[cmd_args_len - 1] = NULL;

    int status = 0;
    for (size_t i = 0; i < arg_i; i++) {
        pid_t ch_pid = fork();
        if (ch_pid == 0) {
            cmd_args[cmd_args_len - 2] = stdin_args[i];
            if (execvp(cmd_args[0], cmd_args) == -1) {
                perror("execvp");
                return EXIT_FAILURE;
            }
        } else if (ch_pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        } else {
            int ch_status;
            waitpid(ch_pid, &ch_status, 0);
            status |= WEXITSTATUS(ch_status);
        }
    }
    free(cmd_args);

    return status;
}
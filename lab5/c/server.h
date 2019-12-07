#include "lib.h"
#include <signal.h>


void sig_handler(int sig);
void print_dt_msg(FILE *f, char *msg);
void update_info(struct info *info, time_t start_time);
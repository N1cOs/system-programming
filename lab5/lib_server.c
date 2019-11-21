#include "server.h"

void print_dt_msg(FILE *f, char *msg) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t); 
    
    char buf[20];
    strftime(buf, 20, "%d-%m-%Y %T", tm);
    
    fprintf(f, "[%s] %s\n", buf, msg);
}

void update_info(struct info *info, time_t start_time) {
    double temp[3] = {0};
    int n = getloadavg(temp, 3);
    if (n == -1) {
        print_dt_msg(stderr, "Can't get info about average load");
    }
    
    info->elapsed = time(NULL) - start_time;
    info->load_1 = temp[0];
    info->load_5 = temp[1];
    info->load_15 = temp[2];
}
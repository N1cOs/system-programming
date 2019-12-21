#include "logs.h"

void print_dt_msg(FILE *f, char *msg) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t); 
    
    char buf[20];
    strftime(buf, 20, "%d-%m-%Y %T", tm);
    fprintf(f, "[%s] %s", buf, msg);
    fflush(f);
}
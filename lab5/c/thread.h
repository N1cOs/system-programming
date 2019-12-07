#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>

#define ALPHABET_LEN 26
 
extern char alphabet[ALPHABET_LEN];

void invert_case();
void reverse();
void print_alphabet();
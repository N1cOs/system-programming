#include "thread.h"
#include <ctype.h>

char alphabet[ALPHABET_LEN] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

static void _invert_case(char data[], size_t length) {
    int (*invert_func)(int);
    if (islower(data[0])) {
        invert_func = toupper;
    } else {
        invert_func = tolower;
    }
    
    for (size_t i = 0; i < length; i++) {
        data[i] = invert_func(data[i]);
    }
}

static void _reverse(char data[], size_t length) {
    char temp[length];
    for (size_t i = 0; i < length; i++) {
        temp[i] = data[i];
    }

    for (size_t i = 0; i < length; i++) {
        data[i] = temp[length - i - 1];
    }
}

static void _print_array(char data[], size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%c ", data[i]);
    }
    printf("\n");
    fflush(stdout);
}

void invert_case() {
    _invert_case(alphabet, ALPHABET_LEN);
}

void reverse() {
    _reverse(alphabet, ALPHABET_LEN);
}

void print_alphabet() {
    _print_array(alphabet, ALPHABET_LEN);
}
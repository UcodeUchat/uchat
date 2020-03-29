#include "../inc/uchat.h"

char *mx_read_client_input(int fd) {
    char *input = mx_strnew(1024);

    read(fd, input, 1024);
    return input;
}
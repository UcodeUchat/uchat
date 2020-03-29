#include "../inc/uchat.h"

int mx_init_info(t_info **info, int argc, char **argv) {
    if (argc != 3)
        mx_err_exit("usage: [ip] [port]\n");
    *info = (t_info *)malloc(sizeof(t_info));
    memset(*info, 0, sizeof(t_info));
    (*info)->argc = argc;
    (*info)->argv = argv;
    (*info)->ip = argv[1];
    (*info)->port = (uint16_t) atoi(argv[2]);
    return 0;
}

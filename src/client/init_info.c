#include "inc/uchat.h"

int mx_init_info_client(t_info_client **info, int argc, char **argv) {
    if (argc != 3)
        mx_err_exit("usage: [ip] [port]\n");
    *info = (t_info_client *)malloc(sizeof(t_info_client));
    memset(*info, 0, sizeof(t_info_client));
    (*info)->argc = argc;
    (*info)->argv = argv;
    (*info)->ip = argv[1];
    (*info)->port = (uint16_t) atoi(argv[2]);
    return 0;
}

#include "uchat.h"


int main(int argc, char **argv) {
    t_info *info = NULL;
    struct servent *servent;

    if (argc != 2)
        mx_err_exit("usage: chat_server [port]\n");

    info = (t_info *)malloc(sizeof(t_info));
    memset(info, 0, sizeof(t_info));
//    init_db(*info);
    (*info).argc = argc;
    (*info).argv = argv;
    (*info).port = (uint16_t) atoi(argv[1]);
    if ((*info).port < 1024)
        mx_err_exit("port id must be minimum 1024\n");
    if ((servent = getservbyport((*info).port, "tcp")) != NULL
        || (servent = getservbyport((*info).port, "udp")) != NULL)
        mx_err_exit("port was taken by another process\n");
    pthread_mutex_init(&((*info).mutex), NULL);
    printf("Configuring .\n");

    if (mx_set_daemon(info) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }

    printf("Configuring demon ++...\n");
    if (mx_start_server(info) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    return 0;
}



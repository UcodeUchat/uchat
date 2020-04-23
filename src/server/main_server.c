#include "uchat.h"

static void init_db(t_server_info *info) {
    int status = sqlite3_open(MX_PATH_TO_DB, &(info->db));

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(info->db));
        exit(1);
    }
}

int main(int argc, char **argv) {
    t_server_info *info = NULL;
    struct servent *servent;

    if (argc != 2)
        mx_err_exit("usage: chat_server [port]\n");
    info = (t_server_info *)malloc(sizeof(t_server_info));
    memset(info, 0, sizeof(t_server_info));
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
//    if (mx_set_daemon(info) == -1) {
//        printf("error = %s\n", strerror(errno));
//        return -1;
//    }
    printf("Configuring demon ++...\n");
    init_db(info);
    if (mx_start_server(info) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    sqlite3_close(info->db);
    return 0;
}



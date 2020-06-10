#include "uchat.h"

static void create_download_folder() {
    struct stat st;

    if (stat(MX_SAVE_FOLDER_IN_SERVER, &st) == -1) {
        mkdir(MX_SAVE_FOLDER_IN_SERVER, 0700);
    }
}

static void init_db(t_server_info *info) {
    int status = sqlite3_open(MX_PATH_TO_DB, &(info->db));

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(info->db));
        exit(1);
    }
}

static void zero_sockets(t_server_info *info) {
    char command[1024];

    sprintf(command, "UPDATE users SET socket='0' WHERE NOT socket='0';");
    sqlite3_exec(info->db, command, NULL, NULL, NULL);
}

static void init_server_info(int argc, char **argv, t_server_info *info) {
    struct servent *servent;

    (*info).argc = argc;
    (*info).argv = argv;
    (*info).port = (uint16_t) atoi(argv[1]);
    if ((*info).port < 1024)
        mx_err_exit("port id must be minimum 1024\n");
    if ((servent = getservbyport((*info).port, "tcp")) != NULL
        || (servent = getservbyport((*info).port, "udp")) != NULL)
        mx_err_exit("port was taken by another process\n");
}

int main(int argc, char **argv) {
    t_server_info *info = NULL;

    if (argc != 2)
        mx_err_exit("usage: chat_server [port]\n");
    info = (t_server_info *)malloc(sizeof(t_server_info));
    memset(info, 0, sizeof(t_server_info));
    init_server_info(argc, argv, info);
    pthread_mutex_init(&((*info).mutex), NULL);
    // if (mx_set_daemon(info) == -1)
    //     mx_err_return3("error: ", strerror(errno), -1);
    init_db(info);
    zero_sockets(info);
    create_download_folder();
    if (mx_start_server(info) == -1)
        mx_err_return3("error: ", strerror(errno), -1);
    sqlite3_close(info->db);
    return 0;
}

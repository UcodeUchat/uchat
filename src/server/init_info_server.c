#include "inc/uchat.h"

// static void sig_init() {
//     signal(SIGINT, mx_sigio_handler);
//     signal(SIGIO, mx_sigio_handler);
//     signal(SIGQUIT, SIG_IGN);
//     signal(SIGTSTP, mx_sigio_handler);
// }

static void init_db(t_info *info) {
    int status = sqlite3_open(MX_PATH_TO_DB, &(info->db));

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(info->db));
        exit(1);
    }
}

void mx_sigio_handler(int sigio) {
    mx_printchar('\n');
    (void)sigio;
}

int mx_init_info_server(t_info **info, int argc, char **argv) {
    struct servent *servent;

    if (argc != 2)
        mx_err_exit("usage: [port]\n");
    // sig_init();
    *info = (t_info *)malloc(sizeof(t_info));
    memset(*info, 0, sizeof(t_info));
    init_db(*info);
    (*info)->argc = argc;
    (*info)->argv = argv;
    (*info)->port = (uint16_t) atoi(argv[1]);
    if ((*info)->port < 1024)
        mx_err_exit("port id must be minimum 1024\n");
    if ((servent = getservbyport((*info)->port, "tcp")) != NULL
        || (servent = getservbyport((*info)->port, "udp")) != NULL)
        mx_err_exit("port was taken by another process\n");
    pthread_mutex_init(&((*info)->mutex), NULL);
    return 0;
}

void mx_clear_form(t_clients *client_info) {
    mx_strdel(&(client_info->login));
    mx_strdel(&(client_info->password));
    mx_strdel(&(client_info->err_msg));
    sqlite3_free(client_info->err_msg);
}

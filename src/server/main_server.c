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
//    int fd = open("lock", O_CREAT | O_RDWR, S_IRWXU);
//    if (fd == -1)
//        printf("error open lock\n");
//
//    int res = mx_set_lock_to_file(fd);
//    printf("return set_lock =%d\n", res);
//    if (res >=0) {
//        printf("write\n");
//       int a = write(fd, "AAAAA", 6);
//       printf("return write=%d\n", a);
//    }

//    mx_set_lock_to_file(fd);

    mx_set_daemon2("log_file");
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

int mx_set_lock_to_file(int fd) {
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    if (fcntl(fd, F_SETLK, &fl) == -1)
        return -1;
    if (fl.l_type == F_UNLCK)
        printf("no lock on this region\n");
    else
        printf("process %d holds the lock\n", (int)fl.l_pid);
    return 0;

//   return fcntl(fd, F_SETLK, &fl);
}

/*
 *     FILE *file;&fl
    int fd;
    if ((fd = open("block", O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, S_IRWXU)) == -1)
        mx_err_exit("open error\n");
    file = fdopen(fd, "w");
//    mx_lockfile(fd);
        flockfile(file);

    if(ftrylockfile(file) != 0) {
        printf("error lock2");
        exit(1);
    }
 */

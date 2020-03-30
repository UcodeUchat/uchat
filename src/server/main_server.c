#include "inc/uchat.h"

// #define LOCKFILE "/var/run/daemon.pid"
// #define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

// extern int lockfile(int);

// int already_running(void) {
//     int fd;
//     char buf[16];
//     fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
//     if (fd < 0) {
//         syslog(LOG_ERR, "невозможно открыть %s: %s", LOCKFILE, strerror(errno));
//         exit(1);
//     }
//     if (lockfile(fd) < 0) {
//         if (errno == EACCES || errno == EAGAIN) {
//             close(fd);
//             return (1);
//         }
//         syslog(LOG_ERR, "невозможно установить блокировку на %s: %s", LOCKFILE, strerror(errno));
//         exit(1);
//     }
//     ftruncate(fd, 0);
//     sprintf(buf, "%ld", (long)getpid());
//     write(fd, buf, strlen(buf) + 1);
//     return (0);
// }

int main(int argc, char **argv) {
    t_info *info = NULL;

    mx_init_info_server(&info, argc, argv);
    printf("Configuring .\n");

    mx_daemonize(info);
    /*
    if (mx_daemonize(info) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
     */
    printf("Configuring demon ++...\n");
    if (mx_start_server(info) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

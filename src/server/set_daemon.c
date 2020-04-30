#include "uchat.h"

void mx_set_daemon2(const char *log_file) {
    int fd;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
//    (void)info;

    umask(0);  // Сбросить маску режима создания файла.
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        mx_err_exit("невозможно получить максимальный номер дескриптора");
    }
    if((pid = fork()) < 0) {
        perror("error fork");
        exit(1);
    }
    if (pid > 0) {
        exit(0);
    }
    setsid();
//     Обеспечить невозможность обретения управляющего терминала в будущем.
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        mx_printerr("невозможно игнорировать сигнал SIGHUP");

    if ((pid = fork()) < 0)
        perror("error fork");
    else if (pid != 0) // родительский процесс
        exit(0);

    // Закрыть все открытые файловые дескрипторы.
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (rlim_t i = 0; i < rl.rlim_max; i++)
        close(i);
    //    if (chdir("/") < 1)
    //        mx_printerr("%s: невозможно сделать текущим рабочим каталогом /\n");

    if ((fd = open(log_file, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, S_IRWXU)) == -1)
        mx_err_exit("open error\n");

    printf("log_file fd  %d\n", fd);
//    openlog("uchat", LOG_CONS, LOG_DAEMON);

    if (dup2(fd, STDOUT_FILENO) == -1
        || dup2(fd, STDERR_FILENO) == -1) {
        syslog(LOG_ERR, "dup error");
        mx_err_exit("dup error\n");
        close(fd);
        exit (0);
    }
    close(STDIN_FILENO);
    close(STDERR_FILENO);
//    return setsid();
}

/*
int mx_set_daemon(t_server_info *info) {
    int fd;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    (void)info;

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        mx_err_exit("невозможно получить максимальный номер дескриптора");
    if((pid = fork()) < 0) {
        perror("error fork");
        exit(1);
    }
    if (pid > 0)
        exit(0);
    umask(0);  // Сбросить маску режима создания файла.

//     Обеспечить невозможность обретения управляющего терминала в будущем.
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        mx_printerr("невозможно игнорировать сигнал SIGHUP");

    // Закрыть все открытые файловые дескрипторы.
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (rlim_t i = 0; i < rl.rlim_max; i++)
        close(i);
    //    if (chdir("/") < 1)
    //        mx_printerr("%s: невозможно сделать текущим рабочим каталогом /\n");

    if ((fd = open("log_file", O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, S_IRWXU)) == -1)
        mx_err_exit("open error\n");

    printf("log_file fd  %d\n", fd);
    if (dup2(fd, STDOUT_FILENO) == -1
        || dup2(fd, STDERR_FILENO) == -1) {
        mx_err_exit("dup error\n");
        close(fd);
        return 0;
    }
    close(STDIN_FILENO);
    close(STDERR_FILENO);
    return setsid();
}
*/

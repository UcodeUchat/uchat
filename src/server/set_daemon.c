#include "uchat.h"



int mx_set_daemon(t_server_info *info) {
    int fd;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    (void)info;

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        mx_err_return("невозможно получить максимальный номер дескриптора");
    if((pid = fork()) < 0)
        mx_err_return("error fork");
    if (pid > 0)
        exit(0);
    umask(0);
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        mx_printerr("невозможно игнорировать сигнал SIGHUP");
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (rlim_t i = 0; i < rl.rlim_max; i++)
        close(i);
    if ((fd = open("log_file", O_CREAT | O_WRONLY | O_TRUNC | O_APPEND,
                   S_IRWXU)) == -1)
        mx_err_return("open error\n");
    if (dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1) {
        close(fd);
        mx_err_return("dup error\n");
    }
    close(STDIN_FILENO);
    close(STDERR_FILENO);
    return setsid();
}


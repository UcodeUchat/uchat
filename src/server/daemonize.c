#include "inc/uchat.h"

int mx_daemonize(t_info *info) {
    int fd;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    (void)info;

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
//    if (chdir("/") < 0)
//        mx_printerr("%s: невозможно сделать текущим рабочим каталогом /\n");

    if ((fd = open(REPORT_FILENAME, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, S_IRWXU)) == -1)
        mx_err_exit("open error\n");
//    printf("log_file fd  %d\n", fd);
    if (dup2(fd, STDOUT_FILENO) == -1
        || dup2(fd, STDERR_FILENO) == -1) {
        mx_err_exit("dup error\n");
        close(fd);
    }
    close(STDIN_FILENO);
    close(STDERR_FILENO);
    return setsid();
}

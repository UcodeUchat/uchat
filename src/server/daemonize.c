#include "inc/uchat.h"

void mx_daemonize(t_info *info) {
//    int fd;
    int fd0;
    int fd1;
    int fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    (void)info;

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        mx_err_exit("невозможно получить максимальный номер дескриптора");

    printf("demon 1 .\n");
    if((pid = fork()) < 0) {
        perror("error fork");
        exit(1);
    }
    if (pid > 0)
        exit(0);
    umask(0);  // Сбросить маску режима создания файла.

//     Обеспечить невозможность обретения управляющего терминала в будущем.

    if ((pid = fork()) < 0)
        mx_printerr("%s: ошибка вызова функции fork\n");
    else if (pid != 0) /* родительский процесс */
        exit(0);
    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    printf("demon 2 .\n");
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        mx_printerr("невозможно игнорировать сигнал SIGHUP");
    // Закрыть все открытые файловые дескрипторы.

    if ((pid = fork()) < 0)
        mx_printerr("%s: ошибка вызова функции fork\n");

    else if (pid != 0) /* родительский процесс */
        exit(0);

    //    if (chdir("/") < 0)
//        mx_printerr("%s: невозможно сделать текущим рабочим каталогом /\n");


    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (rlim_t i = 0; i < rl.rlim_max; i++)
        close(i);

    /*
* Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null. */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
/*
* Инициализировать файл журнала. */
    openlog("uchat_server", LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d", fd0, fd1, fd2);
        exit(1); }

    /*
    printf("demon 3.\n");
    if ((fd = open("logfile", O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, S_IRWXU)) == -1)
        mx_err_exit("open error\n");
    printf("log_file fd  %d\n", fd);
    if (dup2(fd, STDOUT_FILENO) == -1
        || dup2(fd, STDERR_FILENO) == -1) {
        mx_err_exit("dup error\n");
        close(fd);
    }
    close(STDIN_FILENO);
    close(STDERR_FILENO);

     */
//    return setsid();
}

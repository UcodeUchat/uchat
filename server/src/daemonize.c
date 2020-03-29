#include "../inc/uchat.h"

void mx_daemonize(t_info *info) {
    int report_file;
    int dup_fd;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    /* Сбросить маску режима создания файла. */
    umask(0);

    /* Получить максимально возможный номер дескриптора файла. */
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        mx_err_exit("%s: невозможно получить максимальный номер дескриптора\n");

    /* Стать лидером нового сеанса, чтобы утратить управляющий терминал. */
    if ((pid = fork()) < 0)
        mx_err_exit("%s: ошибка вызова функции fork\n");
    else if (pid != 0) /* родительский процесс */
        exit(0);
    setsid();
    
    /* Обеспечить невозможность обретения управляющего терминала в будущем. */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        mx_err_exit("%s: невозможно игнорировать сигнал SIGHUP\n");
    if ((pid = fork()) < 0)
        mx_err_exit("%s: ошибка вызова функции fork\n");
    else if (pid != 0) /* родительский процесс */
        exit(0);
    printf("Start\n");
    /* Закрыть все открытые файловые дескрипторы. */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (int i = 0; i < rl.rlim_max; i++)
        close(i);

    /* Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null. */
    if ((report_file = open(REPORT_FILENAME, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, S_IRWXU)) == -1)
        mx_err_exit("open error\n");
    if (dup2(report_file, STDOUT_FILENO) == -1
        || dup2(report_file, STDERR_FILENO) == -1)
        mx_err_exit("dup error\n");
    // close(report_file);
    printf("pid=%d\n", getpid());
    /* Инициализировать файл журнала. */
}

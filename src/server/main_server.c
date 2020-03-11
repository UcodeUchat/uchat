#include "uchat.h"

int mx_set_demon(const char *log_file) {
    int fd;
    pid_t pid;
    struct rlimit rl;
//    struct sigaction sa;

    if((pid = fork()) < 0) {
        perror("error fork");
        exit(1);
    }
    if (pid > 0)
        exit(0);

//    sa.sa_handler = SIG_IGN;
//    sigemptyset(&sa.sa_mask);
//    sa.sa_flags = 0;
//    if (sigaction(SIGHUP, &sa, NULL) < 0)
//        mx_printerr("невозможно игнорировать сигнал SIGHUP");

     // Закрыть все открытые файловые дескрипторы.
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (rlim_t i = 0; i < rl.rlim_max; i++)
        close(i);

    if ((fd = open(log_file, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, S_IRWXU)) == -1) {
        printf("open error\n");
        return -1;
    }

    printf("log_file fd  %d\n", fd);
    int rc = dup2(fd, STDOUT_FILENO);
    rc = dup2(fd, STDERR_FILENO);
    close(fd);
    if (rc == -1) {
        printf("dup error\n");
        return -1;
    }
    close(STDIN_FILENO);
    close(STDERR_FILENO);
    return setsid();
}

int main(int argc, const char **argv) {
    uint16_t port = atoi(argv[1]);
    int server;

    if (argc < 2) {
        mx_printerr("usage: chat_server [port]\n");
        _exit(1);
    }
    if (mx_set_demon("logfile") == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton("10.112.13.9", &addr.sin_addr);
    if (bind(server, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }
    if (listen(server, SOMAXCONN) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, 1);
    while (1) {
        pthread_t worker_thread;
        int client_sock = accept(server, NULL, NULL);

        printf("server new client socket %d\n", client_sock);
        if (client_sock == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        int rc = pthread_create(&worker_thread, &attr, mx_worker, &client_sock);
        if (rc != 0) {
            printf("error pthread_create = %s\n", strerror(rc));
            close(client_sock);
        }
    }
    pthread_attr_destroy(&attr);
    close(server);
    return 0;
}


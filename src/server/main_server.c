#include "uchat.h"


int mx_set_demon(const char *log_file) {
    int fd;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

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

int main(int argc, char **argv) {



main2(argc, argv);


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

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, argv[1], &hints, &bind_address);


char abuf[INET_ADDRSTRLEN];
const char *addr;
struct sockaddr_in *sinp;

sinp = (struct sockaddr_in *)bind_address->ai_addr;
addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf, INET_ADDRSTRLEN);

    printf("bind_adress %s\n", addr);

    printf("Creating socket...\n");
    server = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (server == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_aton("192.168.174.128", &serv_addr.sin_addr);
    if (bind(server, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }

    // if (bind(server, bind_address->ai_addr, bind_address->ai_addrlen)) {
        // printf("bind error = %s\n", strerror(errno));
        // return -1;
    // }
    freeaddrinfo(bind_address);

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

/*
        // Конвертирует имя хоста в IP адрес
    server = gethostbyname(argv[0]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-1);
    }
    // Указаваем адрес IP сокета
    bcopy((char *)server->h_addr,
          (char *)&_addr.sin_addr.s_addr,
          server->h_length);
    */

    /*
    struct hostent *server;
     int n;
//     char *host;
     if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
        n = HOST_NAME_MAX; // лучшее, что можно сделать
    if ((host = malloc(n)) == NULL)
        err_sys("ошибка вызова функции malloc");
    if (gethostname(host, n) < 0)
        err_sys("ошибка вызова функции gethostname");

*/

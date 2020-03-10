#include "uchat.h"



int mx_daemonize(const char *log_file) {
    if (fork() > 0)
        exit(0);
    int fd = open(log_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd == -1) {
        printf("open error\n");
        return -1;
    }
    int rc = dup2(fd, STDOUT_FILENO);
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

    if (mx_daemonize("server_logfile") == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton("127.0.0.1", &addr.sin_addr);

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
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    while (1) {
        pthread_t worker_thread;
        int client_sock = accept(server, NULL, NULL);
        if (client_sock == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        int rc = pthread_create(&worker_thread, &attr, mx_worker,
                (void *) &client_sock);
        if (rc != 0) {
            printf("error = %s\n", strerror(rc));
            close(client_sock);
        }
    }
    pthread_attr_destroy(&attr);
    close(server);
    return 0;
}


/*

int mx_interact(int client_sock) {
    int buffer = 0;
    ssize_t size = read(client_sock, &buffer, sizeof(buffer));

    if (size <= 0)
        return (int) size;
    printf("Received %d\n", buffer);
    buffer++;
    size = write(client_sock, &buffer, sizeof(buffer));
    if (size > 0)
        printf("Sent %d\n", buffer);
    return (int) size;
}

*/

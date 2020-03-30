#include "inc/uchat.h"

int mx_start_server(t_info *info) {
    int listen_fd;
//    struct sockaddr_in sa;
    struct addrinfo hints;
    struct addrinfo *bind_address;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(0, info->argv[1], &hints, &bind_address);
    listen_fd = socket(bind_address->ai_family,
                    bind_address->ai_socktype, bind_address->ai_protocol);
    if (listen_fd == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(info->port);
    inet_aton("192.168.1.124", &serv_addr.sin_addr);
    if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }

    if (listen(listen_fd, SOMAXCONN) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }

    printf("listen fd = %d\n", listen_fd);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, 1);

    while (1) {
        pthread_t client_thread;
        int client_sock = accept(listen_fd, NULL, NULL);
        int thread_error;
    
        printf("server new client socket %d\n", client_sock);
        if (client_sock == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        pthread_mutex_lock(&info->mutex);
        if ((thread_error = pthread_create(&client_thread, &attr, mx_worker, info)) != 0) {
            printf("error pthread_create = %s\n", strerror(thread_error));
            close(client_sock);
        }
        mx_push_client_back(&(info->clients), client_sock, client_thread);
        pthread_mutex_unlock(&info->mutex);
    }
    pthread_attr_destroy(&attr);
    close(listen_fd);
    return 0;
}



    //     if (connect(sock_fd, NULL, 0) != 0) {
    //         fprintf(stderr, "Connect failed\n");
    //         exit(1);
    //     }
    //     struct sockaddr_in sa;
    //     socklen_t sa_len = sizeof(sa);

    //    if (getsockname(sock_fd, (struct sockaddr *)&sa, &sa_len) == -1) {
    //       perror("getsockname() failed");
    //       exit(1);
    //    }
    //    printf("Local IP address is: %s\n", inet_ntoa(sa.sin_addr));
    //    printf("Local port is: %d\n", (int) ntohs(sa.sin_port));




    // struct servent *servent = getservent();
    // for (int i = 0; i < 100; i++) {
    // mx_print_strarr(servent->s_aliases, "\n");
    // printf("%s\n", servent->s_name);
    // printf("port = %d\n", servent->s_port);
    // printf("proto = %s\n\n", servent->s_proto);
    // printf("============\n");
    // servent = getservent();
    // }
    // servent = getservbyport(1792, "tcp");
    // if (servent) {
    // printf("tcp:\n");
    // printf("%s\n", servent->s_name);
    // printf("port = %d\n", servent->s_port);
    // printf("proto = %s\n\n", servent->s_proto);
    // }
    // else
    //     printf("tcp: not exist\n");
    // servent = getservbyport(1792, "udp");
    // if (servent) {
    // printf("udp:\n");
    // printf("%s\n", servent->s_name);
    // printf("port = %d\n", servent->s_port);
    // printf("proto = %s\n\n", servent->s_proto);
    // }
    // else
    //     printf("udp: not exist\n");




// Take host name:
    // int n;
    // char *host;

    // if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
    //     n = HOST_NAME_MAX; /* лучшее, что можно сделать */
    // if ((host = malloc(n)) == NULL)
    //     mx_err_exit("ошибка вызова функции malloc\n");
    // if (gethostname(host, n) < 0)
    //     mx_err_exit("ошибка вызова функции gethostname\n");
    // host[mx_strlen(host)] = '\0';
    // printf("host=%s\n", host);

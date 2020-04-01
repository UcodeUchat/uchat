#include "uchat.h"

int mx_start_server(t_info *info) {
    int server_socket;
    struct addrinfo hints;
    struct addrinfo *bind_address;
    struct sockaddr_in serv_addr;

    printf("Configuring local address...\n");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(0, info->argv[1], &hints, &bind_address);
    server_socket = socket(bind_address->ai_family,
                       bind_address->ai_socktype, bind_address->ai_protocol);
    if (server_socket == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(info->port);
    inet_aton("192.168.1.124", &serv_addr.sin_addr);
    if (bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }
    if (listen(server_socket, SOMAXCONN) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }

    printf("listen fd = %d\n", server_socket);
////
    char abuf[INET_ADDRSTRLEN];
    const char *addr;
    struct sockaddr_in *sinp;

    sinp = (struct sockaddr_in *)bind_address->ai_addr;
    addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf, INET_ADDRSTRLEN);
    printf("bind_adress %s\n", addr);
////

    freeaddrinfo(bind_address);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, 1);
    while (1) {
        pthread_t worker_thread;
        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);
        char address_buffer[100];

        int client_sock = accept(server_socket, NULL, NULL);
        if (client_sock == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        printf("server new client socket %d\n", client_sock);
        getnameinfo((struct sockaddr*)&client_address, client_len,
                    address_buffer, sizeof(address_buffer), 0, 0,
                    NI_NUMERICHOST);
        printf("New connection from %s\n", address_buffer);

        int rc = pthread_create(&worker_thread, &attr, mx_worker, &client_sock);
        if (rc != 0) {
            printf("error pthread_create = %s\n", strerror(rc));
            close(client_sock);
        }
    }
    pthread_attr_destroy(&attr);
    close(server_socket);
    return 0;
}


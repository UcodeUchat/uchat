#include "uchat.h"

static void s_bind_addr(t_server_info *info, struct addrinfo **bind_address) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(0, info->argv[1], &hints, bind_address);
}

static int create_s_sock(struct addrinfo *bind_address) {
    int server_socket;
    int reuse = 1;

    server_socket = socket(bind_address->ai_family, bind_address->ai_socktype,
                           bind_address->ai_protocol);
    if (server_socket == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse,
                   sizeof(int)) < 0) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }
    return server_socket;
}


int mx_create_server_socket(t_server_info *info) {
    struct addrinfo *bind_address = NULL;
    struct sockaddr_in serv_addr;
    int server_socket;

    s_bind_addr(info, &bind_address);
    if ((server_socket = create_s_sock(bind_address)) == -1)
        return -1;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(info->port);
//    inet_aton("10.111.11.10", &serv_addr.sin_addr);

    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    if (bind(server_socket, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        close(server_socket);
        return -1;
    }
    printf("socket = %d\n", server_socket);
    return server_socket;
}

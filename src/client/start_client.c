#include "uchat.h"

static int connect_client_loop(struct addrinfo *peer_address) {
    int numsec;
    int sock;
    int enable = 1;

    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        if ((sock = socket(peer_address->ai_family, peer_address->ai_socktype,
                           peer_address->ai_protocol)) < 0) {
            freeaddrinfo(peer_address);
            return -1;
        }
        setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
        if ((connect(sock, peer_address->ai_addr,
                     peer_address->ai_addrlen)) == 0)
            return sock;
        printf("not connect\n");
        close(sock);
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }
    return -1;
}



int mx_connect_client(t_client_info *info) {
    struct addrinfo hints;
    struct addrinfo *peer_address = NULL;
    int sock;
    int err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(info->ip, info->argv[2],
                           &hints, &peer_address)) != 0) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(err));
        return -1;
    }
    if ((sock = connect_client_loop(peer_address)) == -1)
        return -1;
    printf("connect to server cocket %d\n", sock);
    return sock;
}

static void clean_client(t_client_info *info) {
    tls_close(info->tls_client);
    tls_free(info->tls_client);
    close(info->socket);
}

int mx_start_client(t_client_info *info) {
    pthread_t thread_input;
    pthread_attr_t attr;
    int tc;

    if (mx_tls_config_client(info))  // conf tls
        return 1;
    info->socket = mx_connect_client(info);
    if(info->socket == -1)
        return 1;
    if (mx_make_tls_connect_client(info)) // tls connect and handshake
        return 1;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // #
    tc = pthread_create(&thread_input, &attr, mx_process_input_from_server, info);
    if (tc != 0)
        printf("pthread_create error = %s\n", strerror(tc));
    mx_login(info);
    pthread_cancel(thread_input);
    clean_client(info);
    return 0;
}

//    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
//        if ((sock = socket(peer_address->ai_family,
//                           peer_address->ai_socktype, peer_address->ai_protocol)) < 0) {
//            freeaddrinfo(peer_address);
//            return -1;
//        }
//        setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
//        if ((connect(sock, peer_address->ai_addr, peer_address->ai_addrlen)) == 0) {
//            printf("connect to server cocket %d\n", sock);
//            return sock;
//        }
//        printf("not connect\n");
//        close(sock);
//        if (numsec <= MAXSLEEP / 2)
//            sleep(numsec);
//    }
//    return -1;

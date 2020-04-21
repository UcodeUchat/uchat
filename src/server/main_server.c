#include "uchat.h"

int main(int argc, char **argv) {
    uint16_t port = atoi(argv[1]);
    struct pollfd pfd[2];
    char bufs[1000], bufc[1000];
    int server;
    unsigned int protocols = 0;

    if (argc != 2) {
        mx_printerr("usage: uchat_server [port]\n");
        _exit(1);
    }
//    if (mx_set_demon("logfile") == -1) {
//        printf("error = %s\n", strerror(errno));
//        return 0;
//    }
    struct tls_config *config = NULL;
    struct tls *tls = NULL;
    struct tls *tls_accept = NULL;
//    char *ciphers = "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384";

    if (tls_init() < 0) {
        printf("tls_init error\n");
        exit(1);
    }
    config = tls_config_new();
    if (config == NULL) {
        printf("error tls_config_new\n");
        exit(1);
    }
//    tls_config_set_protocols(config, TLS_PROTOCOLS_DEFAULT);
//    tls_config_set_ciphers(config, "AEAD-AES256-GCM-SHA384:AEAD-CHACHA20-POLY1305-SHA256:AEAD-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-SHA256");
    tls_config_set_protocols(config, TLS_PROTOCOL_TLSv1_3);
//    tls_config_set_ca_path(config, "./CA3/");

    if(tls_config_parse_protocols(&protocols, "secure") < 0) {
        printf("tls_config_parse_protocols error\n");
        exit(1);
    }
    tls_config_set_protocols(config, protocols);

    if (tls_config_set_ca_file(config, "./CA3/root.pem") < 0) {
        printf("tls_default_ca_cert_file error\n");
        exit(1);
    }
//
//    if(tls_config_set_ciphers(config, ciphers) < 0) {
//        printf("tls_config_set_ciphers error\n");
//        exit(1);
//    }
    if (tls_config_set_key_file(config, "./CA3/server.key") < 0) {
        printf("tls_config_set_key_file error\n");
        exit(1);
    }

    if (tls_config_set_cert_file(config, "./CA3/server.pem") < 0) {
        printf("tls_config_set_cert_file error\n");
        exit(1);
    }
//    tls_config_verify_client_optional(config);
//    tls_config_verify_client(config);
    tls = tls_server();
    if (tls == NULL) {
        printf("tls_server error\n");
        exit(1);
    }
    if (tls_configure(tls, config) < 0) {
        printf("tls_configure error: %s\n", tls_error(tls));
        exit(1);
    }

    printf("2--------++++\n");
    printf("Configuring local address...\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *bind_address;
    getaddrinfo(0, argv[1], &hints, &bind_address);

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
    inet_aton("192.168.1.124", &serv_addr.sin_addr);
//    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }
    freeaddrinfo(bind_address);
    if (listen(server, SOMAXCONN) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }
    printf("listen fd = %d\n", server);
//    freeaddrinfo(bind_address);

    int client_sock = accept(server, NULL, NULL);
    if (client_sock == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }

    if (tls_accept_socket(tls, &tls_accept, client_sock) < 0) {
        printf("tls_accept_socket error\n");
        exit(1);
    }
    printf("tls accept \n");

    char *msg = "tls server";
    tls_write(tls_accept, msg, strlen(msg));

    pfd[0].fd = 0;
    pfd[0].events = POLLIN;
    pfd[1].fd = client_sock;
    pfd[1].events = POLLIN;
    ssize_t len;
    while (bufc[0] != ':' && bufc[1] != 'q') {
        poll(pfd, 2, -1);
        bzero(bufs, 1000);
        bzero(bufc, 1000);
        if (pfd[0].revents & POLLIN) {
            int q = read(0, bufc, 1000);
            tls_write(tls_accept, bufc, q);
        }
        if (pfd[1].revents & POLLIN) {
            if ((len = tls_read(tls_accept, bufs, 1000)) <= 0) break;
            printf("Mensage (%lu): %s\n", len, bufs);
        }
    }
    tls_close(tls_accept);
    tls_free(tls_accept);
    tls_free(tls);
    tls_config_free(config);
    return 0;
}

/*
    while (1) {
        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);
        int client_sock;
        client_sock = accept(server, (struct sockaddr*) &client_address,
                             &client_len);
        printf("server new client socket %d\n", client_sock);
        if (client_sock == -1) {
            printf("error = %s\n", strerror(errno));
//            continue;
            return -1;
        }
        char address_buffer[101];
        getnameinfo((struct sockaddr*)&client_address, client_len,
                    address_buffer, sizeof(address_buffer), 0, 0,
                    NI_NUMERICHOST);
        printf("New connection from %s\n", address_buffer);

        if(tls_accept_socket(tls, &tls_accept, client_sock) < 0) {
            printf("tls_accept_socket error\n");
            exit(1);
        }

        tls_write(tls_accept, "TLS", strlen("TLS"));

}

*/



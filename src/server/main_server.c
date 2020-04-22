#include "uchat.h"

int main(int argc, char **argv) {
    uint16_t port = atoi(argv[1]);
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
//    struct tls *tls_accept = NULL;
    struct tls *tls_accept3 = NULL;
    struct tls *tls_accept4 = NULL;
    struct tls *tls_accept5 = NULL;
    struct tls *tls_accept6 = NULL;
    struct tls *tls_accept7 = NULL;
    struct tls *tls_accept8 = NULL;
    struct tls *tls_accept9 = NULL;

    struct tls *tls_array[10];
    tls_array[3] = tls_accept3;
    tls_array[4] = tls_accept4;
    tls_array[5] = tls_accept5;
    tls_array[6] = tls_accept6;
    tls_array[7] = tls_accept7;
    tls_array[8] = tls_accept8;
    tls_array[9] = tls_accept9;
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
//    tls_config_set_protocols(config, TLS_PROTOCOL_TLSv1_3);
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
    tls_config_verify_client(config);
//    tls_config_verify_client_optional(config);

    tls = tls_server();
    if (tls == NULL) {
        printf("tls_server error\n");
        exit(1);
    }
    if (tls_configure(tls, config) < 0) {
        printf("tls_configure error: %s\n", tls_error(tls));
        exit(1);
    }
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

    int kq;
    if ((kq = kqueue()) == -1) {
        printf("error = %s\n", strerror(errno));
        close(server);
        return -1;
    }
    struct kevent new_ev;  // add server sock in struct kevent
    EV_SET(&new_ev, server, EVFILT_READ, EV_ADD, 0, 0, 0);
    if (kevent(kq, &new_ev, 1, 0, 0, NULL) == -1) {
        printf("error = %s\n", strerror(errno));
        close(server);
        return -1;
    }
    struct timespec timeout;
    timeout.tv_sec = 1;  // seconds
    timeout.tv_nsec = 0;  // nanoseconds
    while (1) {
        int event = kevent(kq, NULL, 0, &new_ev, 1, &timeout);
        if (event == 0) {  // check new event
            continue;
        }
        if (event == -1) {
            printf("error = %s\n", strerror(errno));
            break;
        }
        if (new_ev.ident == (uintptr_t) server) {  // if new connect - add new client
            int client_sock = accept(server, NULL, NULL);
            if (client_sock == -1) {
                printf("error = %s\n", strerror(errno));
                break;
            }
            // print info about new caccpted client
            struct sockaddr_storage client_address;
            socklen_t client_len = sizeof(client_address);
            char address_buffer[100];
            printf("server new client socket %d\n", client_sock);
            getnameinfo((struct sockaddr*)&client_address, client_len,
                        address_buffer, sizeof(address_buffer), 0, 0,
                        NI_NUMERICHOST);
            printf("New connection from %s\n", address_buffer);
            // add client_sock in struct kevent
            EV_SET(&new_ev, client_sock, EVFILT_READ, EV_ADD,0, 0, 0);
            if (kevent(kq, &new_ev, 1, 0, 0, NULL) == -1) {
                printf("error = %s\n", strerror(errno));
                break;
            }

            if(tls_accept_socket(tls, &tls_array[client_sock], client_sock) < 0) {
                printf("tls_accept_socket error\n");
                exit(1);
            }
            if (tls_handshake(tls_array[client_sock]) < 0) {
                printf("tls_handshake error\n");
                printf("%s\n", tls_error(tls_array[client_sock]));
                exit(1);
            }
            mx_report_tls(tls_array[client_sock], "new client connected");
            printf("\n");
//            tls_write(tls_accept, "TLS 1_3", strlen("TLS 1_3"));
//            printf("tls version %s\n", tls_conn_version(tls));
            printf("Client connected successfully\n");
        }
        else {  // if read from client
            printf("\t\t\twork with client %d\n", (int) new_ev.ident);

            if ((new_ev.flags & EV_EOF) != 0) {
//              if ((new_ev.flags & EV_EOF) < 0) {
                    printf("Client %lu disconnected\n", new_ev.ident);
                    //                mx_drop_socket(info, new_ev.ident);
                    close(new_ev.ident);
                    tls_close(tls_array[new_ev.ident]);
                    tls_free(tls_array[new_ev.ident]);
                }
            else {
                int rc = mx_tls_worker(tls_array[new_ev.ident]);
                if (rc == -1) {
                    printf("error = %s\n", strerror(errno));
                    break;
                }
            }
        }
    }
    tls_close(tls);
    tls_free(tls);
    tls_config_free(config);
    return 0;

}

//struct tls *mx_find_tls_cntx(int index) {
//    for (int i = 0; i < 10; i++) {
//        if ()
//    }
//    return NUll;
//}

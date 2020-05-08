#include "uchat.h"

static struct tls *create_tls_configuration(t_server_info *info) {
    struct tls_config *tls_cfg = NULL;
    struct tls *tls;
    uint32_t protocols = 0;
    (void)info;

//    if (tls_init() != 0)  // not
//        mx_err_exit("tls_init error\n");
    if ((tls_cfg = tls_config_new()) == NULL)
        mx_err_exit("unable to allocate tls_cnfg");
    if(tls_config_parse_protocols(&protocols, "secure") != 0)
        mx_err_exit("tls_config_parse_protocols error\n");
    if(tls_config_set_protocols(tls_cfg, protocols) != 0)
        mx_err_exit(tls_config_error(tls_cfg));
    if(tls_config_set_dheparams(tls_cfg, "auto") != 0)
        mx_err_exit(tls_config_error(tls_cfg));
    if (tls_config_set_ca_file(tls_cfg, "./CA/root.pem") != 0)
        mx_err_exit(tls_config_error(tls_cfg));
    if (tls_config_set_key_file(tls_cfg, "./CA/server.key") != 0)
        mx_err_exit(tls_config_error(tls_cfg));
    if (tls_config_set_cert_file(tls_cfg, "./CA/server.pem") != 0)
        mx_err_exit(tls_config_error(tls_cfg));
    tls_config_verify_client(tls_cfg);
    if ((tls = tls_server()) == NULL)
        mx_err_exit("tls server creation failed");
    if (tls_configure(tls, tls_cfg) != 0) {
        printf("tls_configure error: %s\n", tls_error(tls));
        exit(1);
    }
    tls_config_free(tls_cfg);
    return tls;
}

static void make_tls_connect(struct tls *tls, struct tls **tls_sock, int client_sock) {
    printf("client_sock = %d\n", client_sock);
    if(tls_accept_socket(tls, tls_sock, client_sock) < 0) {
        printf("tls_accept_socket error\n");
        exit(1);
    }
    if (tls_handshake(*tls_sock) < 0) {
        printf("tls_handshake error\n");
        printf("%s\n", tls_error(*tls_sock));
        exit(1);
    }
    mx_report_tls(*tls_sock, "new client ");
//    tls_send(tls_sock, "TLS send server", strlen("TLS send server"));
    printf("\nClient connected successfully\n");
}

static int create_server_socket(t_server_info *info) {
    struct addrinfo hints;
    struct addrinfo *bind_address;
    struct sockaddr_in serv_addr;
    int server_socket;

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
    freeaddrinfo(bind_address);
    printf("Configuring local address...\n");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(info->port);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    if (bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }
    return server_socket;
}

int mx_start_server(t_server_info *info) {
    int server;

    struct tls *tls = NULL;
    struct tls *tls_socket = NULL;

    tls = create_tls_configuration(info);
    server = create_server_socket(info);

    if (listen(server, SOMAXCONN) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }
    printf("listen fd = %d\n", server);

    int kq;
    if ((kq = kqueue()) == -1) {
        printf("error = %s\n", strerror(errno));
        close(server);
        return -1;
    }
    // add server sock in struct kevent
    struct kevent new_ev;
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
            struct sockaddr_storage client_address;
            socklen_t client_len = sizeof(client_address);

            int client_sock = accept(server, (struct sockaddr*) &client_address, &client_len);
            if (client_sock == -1) {
                printf("error = %s\n", strerror(errno));
                break;
            }
            printf("server new client socket %d\n", client_sock);
            mx_print_client_address(client_address, client_len);
            // add client_sock in struct kevent
            EV_SET(&new_ev, client_sock, EVFILT_READ, EV_ADD,0, 0, 0);
            if (kevent(kq, &new_ev, 1, 0, 0, NULL) == -1) {
                printf("error = %s\n", strerror(errno));
                break;
            }
            tls_socket = NULL;
            make_tls_connect(tls, &tls_socket, client_sock);
            mx_add_socket_elem(&(info->socket_list), client_sock, tls_socket);
        }
        else {  // if read from client
            printf("\t\t\twork with client %d\n", (int) new_ev.ident);

            if ((new_ev.flags & EV_EOF) != 0) {
                printf("Client %lu disconnected\n", new_ev.ident);
                mx_drop_socket(info, new_ev.ident);
                mx_delete_socket_elem(&(info->socket_list), new_ev.ident);
            }
            else {
                int rc = mx_tls_worker(mx_find_socket_elem(info->socket_list, new_ev.ident), info);
                if (rc == -1) {
                    printf("error = %s\n", strerror(errno));
                    break;
                }
            }
        }
    }
    return 0;
}



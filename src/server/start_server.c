#include "uchat.h"

//#define CERT_FILE "../certs/server-cert.pem"
//#define KEY_FILE  "../certs/server-key.pem"


int mx_start_server(t_server_info *info) {
    int server;
    struct addrinfo hints;
    struct addrinfo *bind_address;
    struct sockaddr_in serv_addr;

    struct tls_config *config = NULL;
    struct tls *tls = NULL;
    struct tls *tls_accept = NULL;
//    char *ciphers = "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384";

    if(tls_init() < 0) {
        printf("tls_init error\n");
        exit(1);
    }
    config = tls_config_new();
    if(config == NULL) {
        printf("error tls_config_new\n");
        exit(1);
    }

    tls_config_set_protocols(config, TLS_PROTOCOLS_DEFAULT);
//    tls_config_set_ciphers(config, "AEAD-AES256-GCM-SHA384:AEAD-CHACHA20-POLY1305-SHA256:AEAD-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-SHA256");
//    tls_config_set_protocols(config, TLS_PROTOCOL_TLSv1_3);
//    tls_config_set_ca_path(config, "./CA3/");

    if (tls_config_set_ca_file(config, "./CA3/root.pem") < 0) {
        printf("tls_default_ca_cert_file error\n");
        exit(1);
    }
//
//    if(tls_config_set_ciphers(config, ciphers) < 0) {
//        printf("tls_config_set_ciphers error\n");
//        exit(1);
//    }

    if(tls_config_set_key_file(config, "./CA3/server.key") < 0) {
        printf("tls_config_set_key_file error\n");
        exit(1);
    }

    if(tls_config_set_cert_file(config, "./CA3/server.pem") < 0) {
        printf("tls_config_set_cert_file error\n");
        exit(1);
    }
//    tls_config_verify_client_optional(config);
    tls_config_verify_client(config);

    tls = tls_server();
    if(tls == NULL) {
        printf("tls_server error\n");
        exit(1);
    }
    if(tls_configure(tls, config) < 0) {
        printf("tls_configure error: %s\n", tls_error(tls));
        exit(1);
    }
    printf("Configuring local address...\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(0, info->argv[1], &hints, &bind_address);
    server = socket(bind_address->ai_family,
                       bind_address->ai_socktype, bind_address->ai_protocol);
    if (server == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(info->port);
    inet_aton("192.168.1.124", &serv_addr.sin_addr);

//    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, 4);
    if (bind(server, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }
    if (listen(server, SOMAXCONN) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }
    printf("listen fd = %d\n", server);
    freeaddrinfo(bind_address);

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

            if(tls_accept_socket(tls, &tls_accept, client_sock) < 0) {
                printf("tls_accept_socket error\n");
                exit(1);
            }

            tls_write(tls_accept, "TLS 1_3", strlen("TLS 1_3"));
//            printf("tls version %s\n", tls_conn_version(tls));
            printf("Client connected successfully\n");
        }
        else {  // if read from client
            printf("\t\t\twork with client %d\n", (int) new_ev.ident);

            if ((new_ev.flags & EV_EOF) != 0) {
                printf("Client %lu disconnected\n", new_ev.ident);
                mx_drop_socket(info, new_ev.ident);
                // close SSL
//                SSL_CTX_free(ctx);
                close(new_ev.ident);
            }
            else {
                int rc = mx_tls_worker(tls_accept, info);
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

SSL_CTX* mx_create_context(void) {
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  // load & register all cryptos, etc.
    SSL_load_error_strings();  // load all error messages
    ctx = SSL_CTX_new(TLS_server_method());  // create new context from method
    if (ctx == NULL ) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void mx_load_certificates(SSL_CTX* ctx, char* cert_file, char* key_file) {
    // set the local certificate from CertFile

    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    // set the private key from KeyFile (may be the same as CertFile)
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    // verify private key
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
    printf("load_cer sucsess\n");
}



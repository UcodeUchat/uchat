#include "uchat.h"

//#define CERT_FILE "../certs/server-cert.pem"
//#define KEY_FILE  "../certs/server-key.pem"


int mx_start_server(t_server_info *info) {
    int server;
    struct addrinfo hints;
    struct addrinfo *bind_address;
    struct sockaddr_in serv_addr;
    SSL_CTX *ctx;


    SSL_library_init();
    ctx = mx_create_context();  // initialize SSL
    mx_load_certificates(ctx,  "mycert.pem", "mycert.pem"); // load cert
    printf("2--------++++\n");

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
    SSL *ssl;
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

            // TLS
            ssl = SSL_new(ctx);
            if (!ctx) {
                fprintf(stderr, "SSL_new() failed.\n");
                return 1;
            }
            printf("SSL/TLS using %s\n", SSL_get_cipher(ssl));
//        mx_show_certs(ssl);
            printf("6--------++++\n");
            SSL_set_fd(ssl, client_sock);

            /* Establish TLS connection */
            int  ret = SSL_accept(ssl);
            if (ret != 1) {
                fprintf(stderr, "SSL_accept error = %d\n",
                        SSL_get_error(ssl, ret));
                return -1;
            }
            printf("Client connected successfully\n");

//            int old_flags  = fcntl(client_sock, F_GETFL, 0);
//            fcntl(client_sock, F_SETFL, old_flags | O_NONBLOCK);
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
                int rc = mx_tls_worker(ssl, new_ev.ident, info);
                if (rc == -1) {
                    printf("error = %s\n", strerror(errno));
                    break;
                }
//        else
//            ERR_print_errors_fp(stderr);
                }

//                int rc = mx_tls_worker(ssl, new_ev.ident, info);
//                int rc = mx_worker(new_ev.ident, info);
//                if (rc == -1) {
//                    printf("error = %s\n", strerror(errno));
//                    break;
        }
    }
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



#include "uchat.h"

int main(int argc, const char **argv) {
    uint16_t port;
    int sock;
    int enable = 1;
    int err;

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        fprintf(stderr, "SSL_CTX_new() failed.\n");
        return 1;
    }
    printf("OpenSSL version: %s\n", OpenSSL_version(SSLEAY_VERSION));
    if (argc < 3) {
        mx_printerr("usage: uchat [ip_adress] [port]\n");
        return -1;
    }
    port = (uint16_t) atoi(argv[2]);
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if ((err = getaddrinfo(argv[1], argv[2], &hints, &peer_address)) != 0) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(err));
        return 1;
    }

    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
            address_buffer, sizeof(address_buffer),
            service_buffer, sizeof(service_buffer),
            NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);
    // sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Creating socket...\n");
    sock = socket(peer_address->ai_family,
                  peer_address->ai_socktype, peer_address->ai_protocol);
    if (sock == -1) {
        printf("error sock = %s\n", strerror(errno));
        return -1;
    }

    setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
    /*
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(argv[1], &addr.sin_addr);

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) { // Connect to server
        printf("connect error = %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    */
    if (connect(sock, peer_address->ai_addr, peer_address->ai_addrlen)) {
        printf("connect error = %s\n", strerror(errno));
        return -1;
    }
    freeaddrinfo(peer_address);

    //SSL
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        fprintf(stderr, "SSL_new() failed.\n");
        return 1;
    }

    SSL_set_fd(ssl, sock);
    printf("SSL_set_fd +\n");
    if (SSL_connect(ssl) <= 0) {
        fprintf(stderr, "SSL_connect() failed.\n");
        ERR_print_errors_fp(stderr);
        return 1;
    }
    else {
        char *msg = "Hello???";
        char buf[1024];
        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        mx_show_certs(ssl);                                /* get any certs */
        SSL_write(ssl, msg, strlen(msg));            /* encrypt & send message */
        int bytes = SSL_read(ssl, buf, sizeof(buf));    /* get reply & decrypt */
        buf[bytes] = 0;
        printf("Received: \"%s\"\n", buf);
    }
    while (1) {
        char read[4096];
        if (!fgets(read, 4096, stdin)) break;
        printf("Sending: %s", read);
        int bytes_sent = SSL_write(ssl, read, strlen(read));
        printf("Sent %d bytes.\n", bytes_sent);

        char server_read[4096];
        int bytes_received = SSL_read(ssl, server_read, 4096);
        if (bytes_received < 1) {
            printf("Connection closed by peer.\n");
            break;
        }
        printf("Received (%d bytes): %.*s",
               bytes_received, bytes_received, read);
        }

//    if (!SSL_set_tlsext_host_name(ssl, argv[1])) {
//        fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");
//        ERR_print_errors_fp(stderr);
//        return 1;
//    }

//    if (SSL_connect(ssl) < 0) {
//        fprintf(stderr, "SSL_connect() failed.\n");
//        ERR_print_errors_fp(stderr);
//        return 1;
//    }
//
//    printf ("SSL/TLS using %s\n", SSL_get_cipher(ssl));

//    mx_show_certs(ssl);

//    int number;
//    while (scanf("%d", &number) > 0) {
//
//        if (SSL_write(ssl, &number, sizeof(number)) == -1) {
//            printf("error write= %s\n", strerror(errno));
//            continue;
//        }
//        printf("Sent %d\n", number);
//        number = 0;
/*
        int bytes_received = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes_received < 1) {
            printf("\nConnection closed by peer.\n");
            break;
        }

        int bytes_sent = SSL_write(ssl, read, strlen(read));
        printf("Sent %d bytes.\n", bytes_sent);


        printf("Received (%d bytes): '%.*s'\n",
               bytes_received, bytes_received, buffer);


*/
//        char read[4096];
        ////
//        int rc = SSL_read(ssl, &number, sizeof(number));
//        if (rc == 0) {
//            printf("Closed connection\n");
//            break;
//        }
//        if (rc == -1)
//            printf("error read= %s\n", strerror(errno));
//        else
//            printf("Received %d\n", number);
//    }
    printf("Closing socket...\n");
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sock);
    return 0;
}

void mx_show_certs(SSL* ssl) {
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}

/*
struct addrinfo *addr;
struct addrinfo filter;
memset(&filter, 0, sizeof(filter));
filter.ai_family = AF_INET;
filter.ai_socktype = SOCK_STREAM;
int rc = getaddrinfo(argv[1], argv[2], &filter, &addr);
if (rc != 0) {
    printf("addrinfo error = %s\n", gai_strerror(rc));
    close(sock);
    return -1;
}
if (addr == NULL) {
    printf("not found a server\n");
    freeaddrinfo(addr);
    close(sock);
    return -1;
}

*/

//
//    int rc = connect(sock, addr->ai_addr, addr->ai_addrlen);
//    freeaddrinfo(addr);
//    if (rc != 0) {
//        printf("connect error = %s\n", strerror(errno));
//        close(sock);
//        return -1;



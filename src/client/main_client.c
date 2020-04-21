#include "uchat.h"

int main(int argc, const char **argv) {
    if (argc < 3) {
        mx_printerr("usage: uchat [ip_adress] [port]\n");
        return -1;
    }
    struct tls *tls = NULL;
    struct tls_config *config = NULL;
    if (tls_init() < 0) {
        printf("tls_init error\n");
        exit(1);
    }
    config = tls_config_new();
    tls = tls_client();
//    tls_config_verify_client_optional(config);
//    tls_config_insecure_noverifycert(config);
//    tls_config_insecure_noverifyname(config);

    if(tls_config_set_key_file(config,  "./CA3/client.key") < 0) {
        printf("tls_config_set_key_file error\n");
        exit(1);
    }
    if(tls_config_set_cert_file(config,  "./CA3/client.pem") < 0) {
        printf("tls_config_set_cert_file error\n");
        exit(1);
    }
    tls_configure(tls, config);

//    printf("connect TCP sock =%d\n", sock);
    tls_connect(tls, argv[1], argv[2]);

//    if (tls_connect_socket(tls, sock, "uchat_server") < 0) {
//        printf("tls_connect error\n");
//        printf("%s\n", tls_error(tls));
//        exit(1);
//    }
    if (tls_handshake(tls) < 0) {
        printf("tls_handshake error\n");
        printf("%s\n", tls_error(tls));
        exit(1);
    }
    tls_write(tls, "TLS client", strlen("TLS client"));
    printf("tls version %s\n", tls_conn_version(tls));
    char bufs[1000], bufc[1000];
    struct pollfd pfd[2];
    ssize_t rc = 0;

    while(bufc[0] != ':' && bufc[1] != 'q') {
        bzero(bufs, 1000);
        bzero(bufc, 1000);

        poll(pfd, 2, -1);

        if(pfd[0].revents & POLLIN) {
            int q = read(0, bufc, 1000);
            tls_write(tls, bufc, q);
        }

        if(pfd[1].revents & POLLIN) {
            if((rc = tls_read(tls, bufs, 1000)) <= 0) break;
            printf("recieve (%lu): %s\n", rc, bufs);
        }

    }
    tls_close(tls);
    tls_free(tls);
    tls_config_free(config);
    printf("Closing socket...\n");
//    close(sock);
    printf("exit client\n");
    return 0;

}

/*
 *     uint16_t port;
    int sock;
    int enable = 1;
    int err;



 *     port = (uint16_t) atoi(argv[2]);
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
    printf("Creating socket...\n");
    sock = socket(peer_address->ai_family,
                  peer_address->ai_socktype, peer_address->ai_protocol);
    if (sock == -1) {
        printf("error sock = %s\n", strerror(errno));
        return -1;
    }
    setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));

//    if (connect(sock, peer_address->ai_addr, peer_address->ai_addrlen)) {
//        printf("connect error = %s\n", strerror(errno));
//        return -1;
//    }
    freeaddrinfo(peer_address);


 */



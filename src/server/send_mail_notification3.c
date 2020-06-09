#include "uchat.h"

static int response_loop(int socket, struct tls *tls) {
    char response[MAXRESPONSE + 1];
    char *p = response;
    char *end = response + MAXRESPONSE;
    int code = 0;
    int bytes_received;

    while (code == 0) {
        bytes_received = socket ? recv(socket, p, end - p, 0)
                                : tls_read(tls, p, end - p);
        if (bytes_received < 1)
            mx_err_return("Connection dropped.\n");
        p += bytes_received;
        *p = 0;
        if (p == end)
            return 1;
        code = mx_check_response(response);
    }
    printf("S: %s", response);
    return code;
}

int mx_connect_to_server(const char *hostname, const char *port) {
    struct addrinfo *peer_address;
    struct addrinfo hints;
    int server;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(hostname, port, &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", strerror(errno));
        return 1;
    }
    server = socket(peer_address->ai_family,
                    peer_address->ai_socktype, peer_address->ai_protocol);
    if (server == -1)
        mx_err_return(strerror(errno));
    if (connect(server, peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%s)\n", strerror(errno));
        return 1;
    }
    freeaddrinfo(peer_address);
    return server;
}

struct tls *mx_create_tls(void) {
    struct tls *tls_c = NULL;
    struct tls_config *config = NULL;

    if ((config = tls_config_new()) == NULL)
        mx_err_exit("unable to allocate config");
    if ((tls_c = tls_client()) == NULL)
        mx_err_exit("tls client creation failed");
    tls_config_insecure_noverifycert(config);
    tls_config_insecure_noverifyname(config);
    if (tls_configure(tls_c, config) != 0) {
        tls_config_free(config);
        printf("tls_configure error: %s\n", tls_error(tls_c));
        exit(1);
    }
    tls_config_free(config);
    return tls_c;
}

int mx_wait_on_response(int socket, struct tls *tls, int reply_code) {
    int code;

    code = response_loop(socket, tls);
    if (code != reply_code) {
        fprintf(stderr, "Error from server:\n");
        return 1;
    }
    return 0;
}

#include "uchat.h"

int mx_start_client(t_info_client *info) {
    char client_input[MAX_CLIENT_INPUT];
    char server_output[MAX_CLIENT_INPUT];
    struct addrinfo hints;
    struct addrinfo *peer_address;
    int sock;
    int err;
    int enable = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(info->ip, info->argv[2], &hints, &peer_address)) != 0) {
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

    sock = socket(peer_address->ai_family,
                  peer_address->ai_socktype, peer_address->ai_protocol);
    if (sock == -1) {
        printf("error sock = %s\n", strerror(errno));
        return -1;
    }
    setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
    if (connect(sock, peer_address->ai_addr, peer_address->ai_addrlen)) {
        printf("connect error = %s\n", strerror(errno));
        return -1;
    }
    freeaddrinfo(peer_address);
    info->fd = sock;


//    int number;
    while (1) {
        mx_get_input(client_input);
        if (strcmp(client_input, "exit") == 0)
            break;

        if (write(sock, client_input, sizeof(client_input)) == -1) {
            printf("error write= %s\n", strerror(errno));
            continue;
        }
        printf("Sent \n");
        int rc = read(sock, server_output, sizeof(server_output));
        if (rc == 0) {
            printf("Closed connection\n");
            break;
        }
        if (rc == -1)
            printf("error read= %s\n", strerror(errno));
        else
            printf("Received %s\n", server_output);
    }
    close(sock);
    return 0;
}

void mx_get_input(char *buffer) {
    int read;

    buffer[0] = 0;
    fgets(buffer, MAX_CLIENT_INPUT, stdin);
    read = strlen(buffer);
    if (read > 0)
        buffer[read - 1] = 0;
}

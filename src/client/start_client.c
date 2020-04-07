#include "uchat.h"

int mx_start_client(t_client_info *info) {
    char client_input[MAX_CLIENT_INPUT];
    char server_output[MAX_CLIENT_INPUT];
    struct addrinfo hints;
    struct addrinfo *peer_address;
    int sock;
    int err;
    int enable = 1;
    int registered = FALSE;
//    char user;

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

    registered = mx_authorization_client(sock);
    printf("registred =%d\n", registered);
//    int number;
    if (registered == 1) {
        while (1) {
            printf("+++\n");
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
    }
    printf("exit client\n");
    close(sock);
    return 0;
}

int mx_authorization_client(int sock) {
    char client_login[MAX_CLIENT_INPUT];
    char client_password[MAX_CLIENT_INPUT];
    char server_output[MAX_CLIENT_INPUT];
    (void)sock;

    printf ("Enter your login: \n");
    int s_login = mx_get_input2(client_login);
    client_login[s_login]= '\0';

    printf ("Enter your password: \n");
    int s_pass = mx_get_input2(client_password);
    client_password[s_pass] = '\0';

    printf ("login: %d: %s\n", s_login, client_login);
    printf ("password: %d: %s\n", s_pass, client_password);

    char *log_pas = mx_strnew(s_login - 1 + s_pass);
//    char tmp;
    log_pas = mx_strjoin(client_login, " ");
    log_pas = mx_strjoin(log_pas, client_password);
//    strncpy(log_pas, client_login, s_login - 1);
//    strcat(log_pas, " ");
//    strcat(log_pas, client_password);
    printf("all %s\n size =%lu \n", log_pas, strlen(log_pas));
    if (write(sock, log_pas, sizeof(log_pas)) == -1) {
        printf("error write= %s\n", strerror(errno));
        return -1;
    }
    int rc = read(sock, server_output, sizeof(server_output));
    if (rc == 0) {
        printf("Closed connection\n");
        return -1;
   }
    if (rc == -1)
        printf("error read= %s\n", strerror(errno));
    server_output[rc] = '\0';
    printf("server output %s\n", server_output);
    if (strcmp(server_output, "login") == 0)
        return 1;
    else
        return -1;

}



#include "uchat.h"

int mx_start_client(t_client_info *info) {
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
    info->socket = sock;

    if (info->auth_client == 0)
        info->auth_client = mx_authorization_client(info);
    if (info->auth_client) {
        while (1) {
            printf("%s\t: ", info->login);
            mx_get_input(client_input);
            if (strcmp(client_input, "exit") == 0)
                break;

            if (write(sock, client_input, sizeof(client_input)) == -1) {
                printf("error write= %s\n", strerror(errno));
                continue;
            }
//            printf("Sent \n");
            int rc = read(sock, server_output, sizeof(server_output));
            if (rc == 0) {
                printf("Closed connection\n");
                break;
            }
            if (rc == -1)
                printf("error read= %s\n", strerror(errno));
            else
                printf("server: \t%s\n", server_output);
        }
    }
    printf("exit client\n");
    close(sock);
    return 0;
}

static char *input_validation(char *login, char *pass) {
    char *new_log = 0;
    char *new_pass = 0;
    char *return_log_pass = mx_strnew(MAX_CLIENT_INPUT * 2);

    new_log = mx_strtrim(login);
    new_pass = mx_strtrim(pass);
    strncat(return_log_pass, login, strlen(login));
    strcat(return_log_pass, " ");
    strncat(return_log_pass, pass, strlen(pass));
    printf("return_log_pass = [%s]\n", return_log_pass);
    return return_log_pass;
}

int mx_authorization_client(t_client_info  *info)  {
    int auth = 0;
    int try = 0;
    int size = 0;
    char login[MAX_CLIENT_INPUT];
    char pass[MAX_CLIENT_INPUT];
    char *to_server = mx_strnew(MAX_CLIENT_INPUT * 2);
    char server_output[128];

    while (try < 3 && auth == 0) {
        printf ("Enter your login: \n");
        mx_get_input(login);

        printf ("Enter your password: \n");
        mx_get_input(pass);
        //проверка на валидность введеных данных, надо доработать
        to_server = input_validation(login, pass);

        if (write(info->socket, to_server, strlen(to_server)) == -1) {
            printf("error write= %s\n", strerror(errno));
            return -1;
        }

        size = read(info->socket, server_output, sizeof(server_output));
        if (size == 0) {
            printf("Closed connection\n");
            return -1;
        }
        if (size == -1)
            printf("error read= %s\n", strerror(errno));
        printf("server_output = [%s]\n", server_output);
        if (mx_strcmp(server_output, "login\0") == 0){
            printf("ZASHLO\n");
            auth = 1;
        }
        try++;
    }
    if (auth)
        return 1;
    return 0;
}


/*
int mx_authorization_client(t_client_info  *info) {
    char client_login[MAX_CLIENT_INPUT];
    char client_password[MAX_CLIENT_INPUT];
    char server_output[MAX_CLIENT_INPUT];

    printf ("Enter your login: \n");
    int s_login = mx_get_input2(client_login);
    client_login[s_login]= '\0';

    printf ("Enter your password: \n");
    int s_pass = mx_get_input2(client_password);
    client_password[s_pass] = '\0';

    printf ("login: %d: %s\n", s_login, client_login);
    printf ("password: %d: %s\n", s_pass, client_password);

    char *log_pas = mx_strnew(s_login - 1 + s_pass);
    log_pas = mx_strjoin(client_login, " ");
    log_pas = mx_strjoin(log_pas, client_password);
//    strncpy(log_pas, client_login, s_login - 1);
//    strcat(log_pas, " ");
//    strcat(log_pas, client_password);
    printf("all %s\n size =%lu \n", log_pas, strlen(log_pas));
    if (write(info->socket, log_pas, sizeof(log_pas)) == -1) {
        printf("error write= %s\n", strerror(errno));
        return -1;
    }
    int rc = read(info->socket, server_output, sizeof(server_output));
    if (rc == 0) {
        printf("Closed connection\n");
        return -1;
   }
    if (rc == -1)
        printf("error read= %s\n", strerror(errno));
    server_output[rc] = '\0';
    printf("server output %s\n", server_output);
    if (strcmp(server_output, "login") == 0) {
        info->login = strdup(client_login);
        return 1;
    }
    else
        return -1;

}
*/


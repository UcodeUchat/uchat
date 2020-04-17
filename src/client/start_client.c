#include "uchat.h"

int mx_start_client(t_client_info *info) {
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
    pthread_t thread_input;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // #
    int tc = pthread_create(&thread_input, &attr, mx_process_input_from_server, info);
    if (tc != 0)
        printf("error = %s\n", strerror(tc));
    mx_print_tid("main thread");
    //-- В этом месте начинается вечный цикл вплоть до закрытия окна чата
    mx_login(info);
    //--
    printf("exit client\n");
    close(sock);
    return 0;
}

static char *input_validation(char *login, char *pass, char **login_for_exit) {
    char *new_log = 0;
    char *new_pass = 0;
    char *return_log_pass = mx_strnew(MAX_CLIENT_INPUT * 2);

    new_log = mx_strtrim(login);
    new_pass = mx_strtrim(pass);
    //for exit 
    *login_for_exit = mx_strnew(MAX_CLIENT_INPUT + 4);
    strcat(*login_for_exit, "exit ");
    strncat(*login_for_exit, login, strlen(login));
   
//    printf("login_for_exit = [%s]\n", *login_for_exit);
    strncat(return_log_pass, login, strlen(login));
    strcat(return_log_pass, " ");
    strncat(return_log_pass, pass, strlen(pass));
    printf("return_log_pass = [%s]\n", return_log_pass);
    return return_log_pass;
}

int mx_authorization_client(t_client_info *info, char **login_for_exit) {
    int auth = 0;
    int try = 0;
    int size = 0;
    char login[MAX_CLIENT_INPUT];
    char pass[MAX_CLIENT_INPUT];
    char *to_server = mx_strnew(MAX_CLIENT_INPUT * 2);
    char server_output[128];
    
    while (try < 3 && auth == 0) {
        printf ("Enter your login: \n");
        int l_size = mx_get_input2(login);

        printf ("Enter your password: \n");
        int p_size = mx_get_input2(pass);
        //проверка на валидность введеных данных, надо доработать
        to_server = input_validation(login, pass, login_for_exit);

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
//            printf("ZASHLO\n");
            login[l_size] = '\0';
            pass[p_size] = '\0';
            info->login = strdup(login);
            auth = 1;
        }
        try++;
    }
    if (auth)
        return 1;
    return 0;
}



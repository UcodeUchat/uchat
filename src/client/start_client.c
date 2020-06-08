#include "uchat.h"

void reconnection_socket(t_client_info *info) {
    json_object *json = mx_create_basic_json_object(MX_RECONNECTION_TYPE);
    const char *json_str = NULL;

    json_object_object_add(json, "login", json_object_new_string(info->login));
    json_object_object_add(json, "password", json_object_new_string(info->password));
    json_str = json_object_to_json_string(json);
    json_str ? tls_send(info->tls_client, json_str, strlen(json_str)) : 0;
}

int mx_reconnect_client(t_client_info *info) {
    int numsec;

    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        printf("reconnect attemp = %d\n", numsec);
        tls_close(info->tls_client);
        tls_free(info->tls_client);
        if (mx_tls_config_client(info))  // conf tls
            return 1;
        info->socket = mx_connect_client(info);
        if (info->socket == -1)
            return 1;
        if ((mx_make_tls_connect_client(info)) == 0) { // tls connect and handshake
            reconnection_socket(info);
            break;
        }
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }
    return 0;
}

int mx_connect_client(t_client_info *info) {
    int numsec;
    struct addrinfo hints;
    struct addrinfo *peer_address = NULL;
    int sock;
    int err;
    int enable = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(info->ip, info->argv[2],
                           &hints, &peer_address)) != 0) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(err));
        return -1;
    }
    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        if ((sock = socket(peer_address->ai_family,
                           peer_address->ai_socktype, peer_address->ai_protocol)) < 0) {
            freeaddrinfo(peer_address);
            return -1;
        }
        setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
        if ((connect(sock, peer_address->ai_addr, peer_address->ai_addrlen)) == 0) {
            printf("connect to server cocket %d\n", sock);
            return sock;
        }
        printf("not connect\n");
        close(sock);
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }
    return -1;
}

int mx_tls_config_client(t_client_info *info) {
    struct tls_config *config = NULL;

    if ((config = tls_config_new()) == NULL)
        mx_err_return("unable to allocate config");
    if ((info->tls_client = tls_client()) == NULL)
        mx_err_return("tls client creation failed");
    tls_config_insecure_noverifycert(config);
    tls_config_insecure_noverifyname(config);
    if (tls_config_set_dheparams(config, "auto") != 0)
        mx_err_return(tls_config_error(config));
    if (tls_config_set_key_file(config, "./CA/client.key") != 0)
        mx_err_return(tls_config_error(config));
    if (tls_config_set_cert_file(config, "./CA/client.pem") != 0)
        mx_err_return(tls_config_error(config));
    if (tls_configure(info->tls_client, config) != 0)
        mx_err_return2("tls_configure error: ", tls_error(info->tls_client));
    tls_config_free(config);
    return 0;
}


int mx_make_tls_connect_client(t_client_info *info) {
    if (tls_connect_socket(info->tls_client, info->socket, "uchat") < 0)
        mx_err_return2("tls_connect error: ", tls_error(info->tls_client));
    if (tls_handshake(info->tls_client) < 0)
        mx_err_return2("tls_handshake error: ", tls_error(info->tls_client));
    printf("tls connect success \n");
    mx_report_tls(info->tls_client, "client");
    return 0;
}

int mx_start_client(t_client_info *info) {
    pthread_t thread_input;
    pthread_attr_t attr;
    int tc;

    if (mx_tls_config_client(info))  // conf tls
        return 1;
    info->socket = mx_connect_client(info);
    if(info->socket == -1)
        return 1;
    if (mx_make_tls_connect_client(info)) // tls connect and handshake
        return 1;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // #
    tc = pthread_create(&thread_input, &attr, mx_process_input_from_server, info);
    if (tc != 0)
        printf("pthread_create error = %s\n", strerror(tc));
    // return ?
    mx_login(info);
    pthread_cancel(thread_input);
    tls_close(info->tls_client);
    tls_free(info->tls_client);
    close(info->socket);
    return 0;
}


/*
static int create_client_socket(t_client_info *info) {
    struct addrinfo hints;
    struct addrinfo *peer_address = NULL;
    int sock;
    int err;
    int enable = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(info->ip, info->argv[2],
                           &hints, &peer_address)) != 0) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(err));
        return 1;
    }
//    printf("Remote address is: ");
//    char address_buffer[100];
//    char service_buffer[100];
//    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
//                address_buffer, sizeof(address_buffer),
//                service_buffer, sizeof(service_buffer),
//                NI_NUMERICHOST);
//    printf("%s %s\n", address_buffer, service_buffer);
    sock = socket(peer_address->ai_family,
                  peer_address->ai_socktype, peer_address->ai_protocol);
    if (sock == -1) {
        printf("error sock = %s\n", strerror(errno));
        return 1;
    }
    setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
    if (connect(sock, peer_address->ai_addr, peer_address->ai_addrlen))
        mx_err_return2("connect error: ",strerror(errno));
//        printf("connect error = %s\n", strerror(errno));
//        return 1;
//    }
    freeaddrinfo(peer_address);
    info->socket = sock;
    return 0;
}
*/


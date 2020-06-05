#include "uchat.h"

#define MAXSLEEP 12

//static int (t_client_info *info, ) {
//
//}


/*
static int create_client_socket(t_client_info *info,  struct addrinfo **peer_address) {
    struct addrinfo hints;
//    struct addrinfo *peer_address = NULL;
    int sock;
    int err;
    int enable = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(info->ip, info->argv[2],
                           &hints, peer_address)) != 0) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(err));
        return 1;
    }
//    printf("Remote address is: ");
//    char address_buffer[100];
//    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);
//    printf("=========%d %d\n", peer_address->ai_family, peer_address->ai_socktype);
    sock = socket(peer_address->ai_family,
                  peer_address->ai_socktype, peer_address->ai_protocol);
    if (sock == -1) {
        freeaddrinfo(*peer_address);
        mx_err_return2("error socket =", strerror(errno));
//        printf("error sock = %s\n", ;
//        return 1;
    }
    setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
    info->socket = sock;
    ////
    if (connect(sock, peer_address->ai_addr, peer_address->ai_addrlen)) {
        close(sock);
        freeaddrinfo(peer_address);
        mx_err_return2("connect error: ", strerror(errno));
    }
    freeaddrinfo(peer_address);
    info->socket = sock;
    return 0;
}

*/
/*
int mx_connect_client_s(t_client_info *info, struct addrinfo *peer_address) {
    if (connect(info->socket, peer_address->ai_addr, peer_address->ai_addrlen)) {
        close(info->socket);
        freeaddrinfo(peer_address);
        mx_err_return2("connect error: ", strerror(errno));
    }
    return 0;
}

*/

void mx_reconect_client(t_client_info *info) {
    int numsec;

    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        printf("reconnect attemp\n");
        tls_close(info->tls_client);
        tls_free(info->tls_client);
        mx_tls_config_client(info);
        info->socket = mx_connect_client(info);
        mx_make_tls_connect_client(info);  // tls connect and handshake
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }
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
        return 1;
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
            printf("no conection\n");
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
//    freeaddrinfo(peer_address);
    tls_close(info->tls_client);
    tls_free(info->tls_client);
    close(info->socket);
    return 0;
}

/*
 *
    (void)info;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = info->port;
    inet_aton(info->ip, &addr.sin_addr);

 */

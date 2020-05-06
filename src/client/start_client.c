#include "uchat.h"

static void create_client_socket(t_client_info *info) {
    struct addrinfo hints;
    struct addrinfo *peer_address = NULL;
    int sock;
    int err;
    int enable = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(info->ip, info->argv[2], &hints, &peer_address)) != 0) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(err));
//        return 1;
        exit(1);
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
//        return -1;
        exit(1);
    }
    setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
    if (connect(sock, peer_address->ai_addr, peer_address->ai_addrlen)) {
        printf("connect error = %s\n", strerror(errno));
//        return -1;
        exit(1);
    }
    freeaddrinfo(peer_address);
    info->socket = sock;
//    return sock;
}

static void set_tls_config(t_client_info *info) {
    struct tls_config *config = NULL;

    if ((config = tls_config_new()) == NULL)
        mx_err_exit("unable to allocate config");
    if ((info->tls_client = tls_client()) == NULL)
        mx_err_exit("tls client creation failed");
    tls_config_insecure_noverifycert(config);
    tls_config_insecure_noverifyname(config);
    if (tls_config_set_dheparams(config, "auto") != 0)
        mx_err_exit(tls_config_error(config));
    if (tls_config_set_key_file(config, "./CA/client.key") != 0)
        mx_err_exit(tls_config_error(config));
    if (tls_config_set_cert_file(config, "./CA/client.pem") != 0)
        mx_err_exit(tls_config_error(config));
    if (tls_configure(info->tls_client, config) != 0) {
        printf("tls_configure error: %s\n", tls_error(info->tls_client));
        exit(1);
    }
    tls_config_free(config);
}
static void make_tls_connect(t_client_info *info) {
    if (tls_connect_socket(info->tls_client, info->socket, "uchat_server") < 0) {
        printf("tls_connect error\n");
        printf("%s\n", tls_error(info->tls_client));
        exit(1);
    }
    printf("tls connect +\n");
    if (tls_handshake(info->tls_client) < 0) {
        printf("tls_handshake error\n");
        printf("%s\n", tls_error(info->tls_client));
        exit(1);
    }
    mx_report_tls(info->tls_client, "client");
    printf("\n");
//    tls_write(info->tls_client, "TLS connect", strlen("TLS connect"));
}

int mx_start_client(t_client_info *info) {

    set_tls_config(info);  // conf tls
    create_client_socket(info);  // socket create and connect
    make_tls_connect(info); // tls connect and handshake

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
    tls_close(info->tls_client);
    tls_free(info->tls_client);
    printf("exit client\n");
    close(info->socket);
    return 0;
}

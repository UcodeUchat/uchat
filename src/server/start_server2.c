#include "uchat.h"

static int tls_config_server(struct tls_config **tls_cfg) {
    uint32_t protocols = 0;

    if(tls_config_parse_protocols(&protocols, "secure") != 0)
        mx_err_return("tls_config_parse_protocols error\n");
    if(tls_config_set_protocols(*tls_cfg, protocols) != 0)
        mx_err_return(tls_config_error(*tls_cfg));
    if(tls_config_set_dheparams(*tls_cfg, "auto") != 0)
        mx_err_return(tls_config_error(*tls_cfg));
    if (tls_config_set_ca_file(*tls_cfg, "./CA/root.pem") != 0)
        mx_err_return(tls_config_error(*tls_cfg));
    if (tls_config_set_key_file(*tls_cfg, "./CA/server.key") != 0)
        mx_err_return(tls_config_error(*tls_cfg));
    if (tls_config_set_cert_file(*tls_cfg, "./CA/server.pem") != 0)
        mx_err_return(tls_config_error(*tls_cfg));
    tls_config_verify_client(*tls_cfg);
    return 0;
}

int mx_create_tls_configuration(struct tls **tls) {
    struct tls_config *tls_cfg = NULL;

    if ((tls_cfg = tls_config_new()) == NULL)
        mx_err_return("unable to allocate tls_cnfg");
    if ((tls_config_server(&tls_cfg)) != 0)
        return 1;
    if ((*tls = tls_server()) == NULL)
        mx_err_return("tls server creation failed");
    if (tls_configure(*tls, tls_cfg) != 0) {
        printf("tls_configure error: %s\n", tls_error(*tls));
        return 1;
    }
    tls_config_free(tls_cfg);
    return 0;
}

int mx_make_tls_connect(struct tls *tls, struct tls **tls_sock,
                        int client_sock) {
    printf("client_sock = %d\n", client_sock);
    if(tls_accept_socket(tls, tls_sock, client_sock) < 0) {
        printf("tls_accept_socket error\n");
        return 1;
    }
    if (tls_handshake(*tls_sock) < 0) {
        printf("tls_handshake error\n");
        printf("%s\n", tls_error(*tls_sock));
        return 1;
    }
    mx_report_tls(*tls_sock, "new client ");
    printf("\nClient connected successfully\n");
    return 0;
}

/*
static int make_bind_server(t_server_info *info,
                            struct sockaddr_in *serv_addr, int server_sock) {

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(info->port);
    inet_aton("127.0.0.1", &serv_addr->sin_addr);
    if (bind(server_sock, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        close(server_sock);
        return -1;
    }
    return 0;
}
*/

int mx_create_server_socket(t_server_info *info) {
    struct addrinfo hints;
    struct addrinfo *bind_address;
    struct sockaddr_in serv_addr;
    int server_socket;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(0, info->argv[1], &hints, &bind_address);
    server_socket = socket(bind_address->ai_family,
                           bind_address->ai_socktype, bind_address->ai_protocol);
    if (server_socket == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }
    freeaddrinfo(bind_address);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(info->port);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    if (bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        close(server_socket);
        return -1;
    }
//    if ((make_bind_server(info, &serv_addr, server_socket)) != 0)
//        return -1;
    return server_socket;
}




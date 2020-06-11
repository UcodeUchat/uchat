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
    return 0;
}

#include "uchat.h"

void reconnection_socket(t_client_info *info) {
    json_object *json = mx_create_basic_json_object(MX_RECONNECTION_TYPE);
    const char *json_str = NULL;

    mx_js_o_o_add(json, "login",
                           mx_js_n_str(info->login));
    mx_js_o_o_add(json, "password",
                           mx_js_n_str(info->password));
    json_str = mx_js_o_to_js_str(json);
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
        if ((mx_make_tls_connect_client(info)) == 0) { // tls connect and han
            reconnection_socket(info);
            break;
        }
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }
    return 0;
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

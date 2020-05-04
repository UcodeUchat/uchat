#include "uchat.h"

int mx_tls_worker(t_socket_list *csl, t_server_info *info) {
    t_package *new_package = malloc(MX_PACKAGE_SIZE);
    int rc;
    char json_string[1024];
    json_object *new_json;

    rc = tls_read(csl->tls_socket, &json_string, 1024);    // get json
    // printf("read json %s\n", json_string);
    new_json = json_tokener_parse(json_string);
    mx_print_json_object(new_json, "mx_tls_worker");
    if (rc == -1) {
        tls_close(csl->tls_socket);
        tls_free(csl->tls_socket);
    }
    if (rc > 0) {
        new_package = mx_json_to_package(new_json);

        printf("\n\nLogin %s\n", new_package->login);
        printf("Password %s\n\n\n", new_package->password);

        new_package->client_sock = csl->socket;
        new_package->client_tls_sock = csl->tls_socket;
        mx_run_function_type(info, csl, new_package, new_json);
    }
    else
        printf("Readed 0 bytes\n");
    free(new_package);
    return 0;
}

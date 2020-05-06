#include "uchat.h"

int mx_process_input_objects(t_server_info *info, t_socket_list *csl, char *buffer, size_t rd) {
    enum json_tokener_error jerr;

    csl->obj = json_tokener_parse_ex(csl->tok, buffer, rd);
    jerr = json_tokener_get_error(csl->tok);

    if (jerr == json_tokener_success) {
        mx_run_function_type(info, csl);
        json_tokener_reset(csl->tok);
        json_object_put(csl->obj);
    }
    else if (jerr == json_tokener_continue) {
        // printf("json_tokener_continue\n");
    }
    else {
        json_tokener_reset(csl->tok);
        json_object_put(csl->obj);
        fprintf(stderr, "Error: %s\n", json_tokener_error_desc(jerr));
        return 1;
    }
    return 0;
}

int mx_tls_worker(t_socket_list *csl, t_server_info *info) {
    size_t readed;
    char *input = malloc(1024);

    readed = tls_read(csl->tls_socket, input, 1024);   // get json
    if (readed > 0)
        mx_process_input_objects(info, csl, input, readed);
    else
        printf("Readed 0 bytes\n");
    return 0;
}

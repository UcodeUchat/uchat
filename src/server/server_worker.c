#include "uchat.h"

int mx_process_input_objects(t_server_info *info, t_socket_list *csl, char *buffer, size_t rd) {
    enum json_tokener_error jerr;

    csl->obj = json_tokener_parse_ex(csl->tok, buffer, rd);
    jerr = json_tokener_get_error(csl->tok);

    printf("parse:%zu, rd:%zu\n", json_tokener_get_parse_end(csl->tok), rd);
    if (jerr == json_tokener_success) {
        mx_run_function_type(info, csl);
        // json_tokener_reset(csl->tok);
        printf("json_tokener_success: %s\n", json_tokener_error_desc(jerr));
    }
    else if (jerr == json_tokener_continue) {
        printf("json_tokener_continue: %s\n", json_tokener_error_desc(jerr));
    }
    else {
        // json_tokener_reset(csl->tok);
        fprintf(stderr, "Error: %s\n", json_tokener_error_desc(jerr));
    }
    json_object_put(csl->obj);
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

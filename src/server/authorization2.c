#include "uchat.h"

static void auth_not_valid(t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;

    mx_js_s_int(mx_js_o_o_get(js, "type"),
                        MX_AUTH_TYPE_NV);
    json_string = mx_js_o_to_js_str(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string,
                 strlen(json_string));
}

static int search_data(void *p, int argc, char **argv, char **col_name) {
    (void)p;
    (void)col_name;
    (void)argv;
    if (argc > 0)
        return 1;
    return 0;
}

int mx_authorization(t_server_info *i, t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;
    int valid = mx_check_client(i, js, csl->socket);
    
    if (valid == 1) {
        mx_js_s_int(mx_js_o_o_get(js, "type"),
                            MX_AUTH_TYPE_V);
        mx_js_o_o_add(js, "room_id", mx_js_n_int(0));
        mx_get_rooms(i, js);
        json_string = mx_js_o_to_js_str(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string,
                     strlen(json_string));
        mx_email_notify(i, js);      
    }
    else
        auth_not_valid(csl, js);
    return 1;
}

int mx_search_in_db(t_server_info *i, const char *l, const char *pa) {
    char command[1024];

    (void)pa;
    sprintf(command, "SELECT login FROM users WHERE login='%s'", l);
    if (sqlite3_exec(i->db, command, search_data, NULL, 0) != SQLITE_OK)
        return -1;
    return 1;
}

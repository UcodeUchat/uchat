#include "uchat.h"

static int get_user_id(void *p, int argc, char **argv, char **col_name) {
    int *id = (int *)p;

    (void)col_name;
    if (argc > 1 || !argv[0])
        return 1;
    *id = atoi(argv[0]);
    return 0;
}

int mx_add_to_db(t_server_info *i, const char *l, const char *pa, int us_id) {
    char command[1024];

    sprintf(command, "insert into users (socket, login, password, access)\
                values (0,'%s', '%s', 1);\nSELECT last_insert_rowid()", l, pa);
    if (sqlite3_exec(i->db, command, get_user_id, &us_id, 0) != SQLITE_OK)
        return -1;
    sprintf(command, "insert into room_user (room_id, user_id)\
                values (0,%d);", us_id);
    if (sqlite3_exec(i->db, command, NULL, NULL, 0) != SQLITE_OK)
        return -1;
    sprintf(command, "insert into user_notifications (user_id, visual, audio,\
            email) values (%d, 0, 0, 0);", us_id);
    if (sqlite3_exec(i->db, command, NULL, NULL, 0) != SQLITE_OK)
        return -1;
    return 1;
}

int mx_registration(t_server_info *i, t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;

    if (mx_search_in_db(i, mx_js_g_str(mx_js_o_o_get(js, \
        "login")), mx_js_g_str(mx_js_o_o_get(js,\
        "password"))) == -1) {
        mx_js_s_int(mx_js_o_o_get(js, "add_info"),\
                                                    MX_AUTH_TYPE_NV);
    }
    else{
        mx_add_to_db(i, mx_js_g_str(mx_js_o_o_get(js,\
        "login")), mx_js_g_str(mx_js_o_o_get(js,\
        "password")), -1);
        mx_js_s_int(mx_js_o_o_get(js, "add_info"),\
                                                    MX_AUTH_TYPE_V);
    }
    json_string = mx_js_o_to_js_str(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string,\
                 strlen(json_string));
    return 1;
}

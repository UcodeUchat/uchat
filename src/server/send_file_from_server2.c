#include "uchat.h"

static int res(void *arg, int argc, char **argv, char **col_name) {
    (void)col_name;
    (void)argc;
    (void)arg;
    if (argv[0])
        return 0;
    return 1;
}

static int get_result(void *arg, int argc, char **argv, char **col_name) {
    json_object *obj = (json_object *)arg;

    (void)col_name;
    (void)argc;
    if (argv[0]) {
        mx_js_o_o_add(obj, "name", mx_js_n_str(argv[3]));
        return MX_OK;
    }
    return 1;
}

t_file_tmp *mx_set_variables(t_socket_list *csl) {
    t_file_tmp *new = malloc(sizeof(t_file_tmp));

    new->tls = csl->tls_socket;
    new->mutex = &csl->mutex;
    new->file_id = mx_js_g_int(mx_js_o_o_get(csl->obj,\
                                        "file_id"));
    new->room_id = mx_js_g_int(mx_js_o_o_get(csl->obj,\
                                        "room_id"));
    return new;
}

int mx_check_is_object_valid(json_object *obj) {
    if (mx_js_o_o_get(obj, "room_id")
        && mx_js_o_o_get(obj, "user_id")
        && mx_js_o_o_get(obj, "file_id"))
        return MX_OK;
    else
        return 1;
}

char *mx_check_file_in_db_user_access(t_server_info *info, json_object *obj) {
    int file_id = mx_js_g_int(mx_js_o_o_get(obj, "file_id"));
    char command[1024];

    command[sprintf(command, "select * from msg_history where id='%d'",
                    file_id)] = '\0';
    if (sqlite3_exec(info->db, command, get_result, obj, NULL) != SQLITE_OK) {
		return NULL;
    }
    command[sprintf(command, "select * from room_user where user_id='%d' & \
            room_id='%d'", mx_js_g_int(mx_js_o_o_get(obj,
            "user_id")), mx_js_g_int(mx_js_o_o_get(obj,
            "room_id")))] = '\0';
    if (sqlite3_exec(info->db, command, res, NULL, NULL) != SQLITE_OK) {
		return NULL;
    }
    return strdup(mx_js_g_str(mx_js_o_o_get(obj, "name")));
}

#include "uchat.h"

static int save_message(t_server_info *info, json_object *js) {
    char command[1024];
    int add_info = mx_js_g_int(mx_js_o_o_get(js, "add_info"));
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
    char *message = mx_replace_substr(mx_js_g_str(\
                            mx_js_o_o_get(js, "data")), "'", "''");

    command[sprintf(command, "INSERT INTO msg_history (user_id, room_id,\
        message, addition_cont) VALUES ('%d', '%d', '%s', '%s'); SELECT\
        last_insert_rowid()", user_id, room_id, message,
        add_info == 0 ? "mes" : "stik")] = '\0';
    mx_strdel(&message);
    if (sqlite3_exec(info->db, command, mx_get_data, js, NULL) != SQLITE_OK) {
        return -1;
    }
    return 1;
}

int mx_get_data(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        struct json_object *t = mx_js_n_int(atoi(argv[0]));
        mx_js_o_o_add((struct json_object*) js, "id", t);
        return 0;
    }
    return 1;
}

int mx_process_message_in_server(t_server_info *info, json_object *js) {
    int res = save_message(info, js);
    if (res > 0)
        mx_send_json_to_all_in_room(info, js);
    else
        printf("loh, ne sohranilos'\n");
    return 0;
}


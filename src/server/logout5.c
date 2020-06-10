#include "uchat.h"

static int get_logins(void *name, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    char **str = (char  **)name;

    if (argv[0]) {
        *str = mx_strjoin(*str, argv[0]);
        return 0;
    }
    return 1;
}

static void get_name(t_server_info *info, char **name, int first_id, int id2) {
    char command[1024];

    sprintf(command, "SELECT login FROM users WHERE id='%d' OR id='%d';",
            first_id, id2);
    sqlite3_exec(info->db, command, get_logins, name, NULL);
}

static void exist (t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;

    mx_js_o_o_add(js, "exist", mx_js_n_int(1));
    json_string = mx_js_o_to_js_str(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
}

static void init_direct (t_server_info *info, json_object *js, char *name) {
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
    char *command3 = malloc(1024);
    int first_id = mx_js_g_int(mx_js_o_o_get(js, "first_id"));
    int second_id = mx_js_g_int(mx_js_o_o_get(js, "second_id"));

    sprintf(command3, "INSERT INTO room_user (room_id, user_id, role) VALUES ('%d', '%d', '0'); \
            INSERT INTO room_user (room_id, user_id, role) VALUES ('%d', '%d', '0'); \
            INSERT INTO direct_rooms (id, first_id, second_id) VALUES ('%d', '%d', '%d');" , 
            room_id, first_id, room_id, second_id, room_id, first_id, second_id);
    if (sqlite3_exec(info->db, command3, NULL, NULL, NULL) == SQLITE_OK) {
        json_object *room_data = mx_js_n_o();;
        mx_js_o_o_add(room_data, "room_id", mx_js_n_int(room_id));
        mx_js_o_o_add(room_data, "name", mx_js_n_str(name));
        json_object *messages = json_object_new_array();
        mx_js_o_o_add(room_data, "access", mx_js_n_int(3));
        mx_js_o_o_add(room_data, "messages", messages);
        mx_js_o_o_add(js, "room_data", room_data);
        mx_js_o_o_add(js, "exist", mx_js_n_int(0));
        mx_send_json_to_all_in_room(info, js);
    }
}

static void not_exist(t_server_info *info, json_object *js, char *name) {
    char *command2 = malloc(1024);

    sprintf(command2, "INSERT INTO rooms (name, access) VALUES ('%s', '3'); \
    SELECT last_insert_rowid();" , name);
    if (sqlite3_exec(info->db, command2, mx_get_room_id, js, NULL) == SQLITE_OK) {
        init_direct(info, js, name);
    }
     mx_strdel(&command2);
}

int mx_direct_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    int first_id = mx_js_g_int(mx_js_o_o_get(js, "first_id"));
    int second_id = mx_js_g_int(mx_js_o_o_get(js, "second_id"));
    char *command = malloc(1024);
    char *name = mx_strnew(1);

    get_name(info, &name, first_id, second_id);
    sprintf(command, "SELECT * FROM rooms INNER JOIN direct_rooms USING(id) \
        WHERE access=3 AND ((first_id='%d' AND second_id='%d') OR (first_id='%d' AND second_id='%d'));",
        first_id, second_id, second_id, first_id);
    sqlite3_exec(info->db, command, mx_get_room_id, js, NULL);
    if (mx_js_g_int(mx_js_o_o_get(js, "room_id")) != 0) {
        exist (csl, js);
    }
    else {
        not_exist(info, js, name);
    }
    mx_strdel(&command);
    return 1;
}

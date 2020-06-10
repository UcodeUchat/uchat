#include "uchat.h"

static void load_jroom_data (t_server_info *info, json_object *js, 
                            int room_id) {
    json_object *room_data = mx_js_o_o_get(js, "room_data");
    json_object *messages = json_object_new_array();
    char *command1 = mx_strnew(1024);

    mx_js_o_o_add(room_data, "messages", messages);
    sprintf(command1, "SELECT *  FROM msg_history, users \
            where room_id = %d and users.id = msg_history.user_id \
            order by msg_history.id desc limit 5;", room_id);
    if (sqlite3_exec(info->db, command1, mx_get_rooms_data, messages, 0)
         == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
    }
    mx_strdel(&command1);
}

int mx_join_room (t_server_info *info, json_object *js) {
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
    char *command = malloc(1024);

    sprintf(command, "INSERT INTO room_user (user_id, room_id, role) \
            VALUES ('%d', '%d', '%d');", user_id, room_id, 0);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        load_jroom_data (info, js, room_id);
    }
    mx_strdel(&command);
    return 1;
}

static void create_room_user (t_server_info *info, t_socket_list *csl, 
                                json_object *js, json_object *room_data) {
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int room_id = mx_js_g_int(mx_js_o_o_get(room_data, "room_id"));
    char *command1 = mx_strnew(1024);
    const char *json_string = NULL;

    mx_js_o_o_add(js, "room_id", mx_js_n_int(room_id));
    sprintf(command1, "INSERT INTO room_user (user_id, room_id, role) \
        VALUES ('%d', '%d', '%d');", user_id, room_id ,1);
    if (sqlite3_exec(info->db, command1, NULL, NULL, 0) == SQLITE_OK) {
        json_string = mx_js_o_to_js_str(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, 
                        strlen(json_string));
    }
    mx_strdel(&command1);
}

int mx_create_room_server (t_server_info *info, t_socket_list *csl, 
                            json_object *js) {
    int access = mx_js_g_int(mx_js_o_o_get(js, "acces"));
    const char *name = mx_js_g_str(mx_js_o_o_get(js, "name"));
    json_object *room_data = mx_js_o_o_get(js, "room_data");
    char *command = malloc(1024);

    sprintf(command, "INSERT INTO rooms (name, access) VALUES ('%s', '%d'); \
        SELECT last_insert_rowid();" , name, access);
    if (sqlite3_exec(info->db, command, mx_get_room_id, room_data, NULL) 
        == SQLITE_OK) {
        create_room_user(info, csl, js, room_data);
    }
    mx_strdel(&command);
    return 1;
}

int mx_get_room_id(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        struct json_object *t = mx_js_n_int(atoi(argv[0]));
        mx_js_o_o_add((struct json_object*) js, "room_id", t);
        return 0;
    }
    return 1;
}

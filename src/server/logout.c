#include "uchat.h"

int mx_leave_room (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
    char command[1024];
    const char *json_string = NULL;

    sprintf(command, "DELETE FROM room_user where user_id\
            ='%d' and room_id='%d';", user_id, room_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
        json_string = mx_js_o_to_js_str(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    }
    return 1;
}

int mx_edit_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    int msg_id = mx_js_g_int(mx_js_o_o_get(js, "message_id"));
    const char *d = mx_js_g_str(mx_js_o_o_get(js, "data"));
    char command[1024];

    (void)csl;
    sprintf(command, "UPDATE msg_history SET message='%s' where id='%d';",
            d, msg_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK)
        mx_send_json_to_all_in_room(info, js);
    return 1;
}


int mx_load_history (t_server_info *info, t_socket_list *csl, json_object *js) {
    const char *json_str = NULL;
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
    int last_id = mx_js_g_int(mx_js_o_o_get(js, "last_id"));
    char command[1024];
    json_object *messages = json_object_new_array();

    mx_js_o_o_add(js, "messages", messages);
    sprintf(command, "SELECT *  FROM msg_history, users where room_id = \
            %d and msg_history.id < %d and users.id = msg_history.user_id \
            order by msg_history.id desc limit 15;", room_id, last_id);
    sqlite3_exec(info->db, command, mx_get_rooms_data, messages, NULL);
    json_str = mx_js_o_to_js_str(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_str, strlen(json_str));
    return 1;
}

int mx_logout (t_server_info *i, t_socket_list *csl, json_object *js) {
    char *command = malloc(1024);
    int id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));

    (void)csl;
    sprintf(command, "UPDATE users SET socket='0' WHERE id='%s'", mx_itoa(id));
    sqlite3_exec(i->db, command, NULL, NULL, NULL);
    mx_strdel(&command);
	return 1;
}

#include "uchat.h"

static void load_user_data_2(char **argv, void **js) {
    json_object *visual_n = mx_js_n_int(atoi(argv[9]));
    json_object *audio_n = mx_js_n_int(atoi(argv[10]));
    json_object *email_n = mx_js_n_int(atoi(argv[11]));

    mx_js_o_o_add(*(struct json_object **)js, "visual_n", visual_n);
    mx_js_o_o_add(*(struct json_object **)js, "audio_n", audio_n);
    mx_js_o_o_add(*(struct json_object **)js, "email_n", email_n);
}

static int load_user_data(void *js, int argc, char **argv, char **col_name) {
    json_object *name = NULL;
    json_object *email = NULL;
    json_object *login = mx_js_n_str(argv[2]);

    (void)argc;
    (void)col_name;
    name = argv[4] ? mx_js_n_str(argv[4])
                    : mx_js_n_str("");
    email = argv[5] ? mx_js_n_str(argv[5])
                    : mx_js_n_str("");
    mx_js_o_o_add((struct json_object *)js, "login", login);
    mx_js_o_o_add((struct json_object *)js, "name", name);
    mx_js_o_o_add((struct json_object *)js, "email", email);
    load_user_data_2(argv, &js);
    return 0;
}

int mx_load_profile (t_server_info *info, t_socket_list *csl, json_object *js) {
    int id = mx_js_g_int(mx_js_o_o_get(js, "id"));
    char cmd[1024];
    const char *json_string = NULL;

    sprintf(cmd, "SELECT * FROM users, user_notifications \
            where users.id='%d' and user_notifications.user_id='%d';", id, id);
    if (sqlite3_exec(info->db, cmd, load_user_data, js, NULL) == SQLITE_OK) {
        json_string = mx_js_o_to_js_str(js);
        mx_save_send(&csl->mutex, csl->tls_socket,
                     json_string, strlen(json_string));
    }
    return 1;
}

static void init_direct1 (t_server_info *info, json_object *js, char *name) {
    json_object *room_data = mx_js_n_o();
    json_object *messages = json_object_new_array();
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));

    mx_js_o_o_add(room_data, "room_id", mx_js_n_int(room_id));
    mx_js_o_o_add(room_data, "name", mx_js_n_str(name));
    mx_js_o_o_add(room_data, "access", mx_js_n_int(3));
    mx_js_o_o_add(room_data, "messages", messages);
    mx_js_o_o_add(js, "room_data", room_data);
    mx_js_o_o_add(js, "exist", mx_js_n_int(0));
    mx_send_json_to_all_in_room(info, js);
}

void mx_init_direct (t_server_info *info, json_object *js, char *name) {
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
    char *command3 = malloc(1024);
    int first_id = mx_js_g_int(mx_js_o_o_get(js, "first_id"));
    int second_id = mx_js_g_int(mx_js_o_o_get(js, "second_id"));

    sprintf(command3, "INSERT INTO room_user (room_id, user_id, role) \
             VALUES ('%d', '%d', '0'); \
            INSERT INTO room_user (room_id, user_id, role) \
            VALUES ('%d', '%d', '0'); \
            INSERT INTO direct_rooms (id, first_id, second_id) \
            VALUES ('%d', '%d', '%d');" , 
            room_id, first_id, room_id, second_id, room_id, first_id, second_id);
    if (sqlite3_exec(info->db, command3, NULL, NULL, NULL) == SQLITE_OK) {
        init_direct1(info, js, name);
    }
}

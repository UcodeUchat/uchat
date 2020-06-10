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

static int get_room_id(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        struct json_object *t = mx_js_n_int(atoi(argv[0]));
        mx_js_o_o_add((struct json_object*) js, "room_id", t);
        return 0;
    }
    return 1;
}

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

int mx_direct_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    int first_id = mx_js_g_int(mx_js_o_o_get(js, "first_id"));
    int second_id = mx_js_g_int(mx_js_o_o_get(js, "second_id"));
    char *command = malloc(1024);
    char *name = mx_strnew(1);
    const char *json_string = NULL;

    get_name(info, &name, first_id, second_id);
    sprintf(command, "SELECT * FROM rooms INNER JOIN direct_rooms USING(id) \
        WHERE access=3 AND ((first_id='%d' AND second_id='%d') OR (first_id='%d' AND second_id='%d'));",
        first_id, second_id, second_id, first_id);
    sqlite3_exec(info->db, command, get_room_id, js, NULL);
    if (mx_js_g_int(mx_js_o_o_get(js, "room_id")) != 0) {
        mx_js_o_o_add(js, "exist", mx_js_n_int(1));
        json_string = mx_js_o_to_js_str(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
        mx_strdel(&command);
    }
    else {
        char *command2 = malloc(1024);

        sprintf(command2, "INSERT INTO rooms (name, access) VALUES ('%s', '3'); \
        SELECT last_insert_rowid();" , name);
        if (sqlite3_exec(info->db, command2, get_room_id, js, NULL) == SQLITE_OK) {
            int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
            char *command3 = malloc(1024);
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
    }
    return 1;
}


int mx_create_room_server (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int access = mx_js_g_int(mx_js_o_o_get(js, "acces"));
    const char *name = mx_js_g_str(mx_js_o_o_get(js, "name"));
    const char *json_string = NULL;
    json_object *room_data = mx_js_o_o_get(js, "room_data");
    char *command = malloc(1024);

    sprintf(command, "INSERT INTO rooms (name, access) VALUES ('%s', '%d'); \
        SELECT last_insert_rowid();" , name, access);
    if (sqlite3_exec(info->db, command, get_room_id, room_data, NULL) == SQLITE_OK) {
        int room_id = mx_js_g_int(mx_js_o_o_get(room_data, "room_id"));
        char *command1 = mx_strnew(1024);

        mx_js_o_o_add(js, "room_id", mx_js_n_int(room_id));

        sprintf(command1, "INSERT INTO room_user (user_id, room_id, role) VALUES ('%d', '%d', '%d');", user_id, room_id ,1);
        if (sqlite3_exec(info->db, command1, NULL, NULL, 0) == SQLITE_OK) {
            json_string = mx_js_o_to_js_str(js);
            mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
            mx_strdel(&command1);
            mx_strdel(&command);
        }
    }
    return 1;
}

int mx_join_room (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int room_id = mx_js_g_int(mx_js_o_o_get(js, "room_id"));
    char *command = malloc(1024);

    (void)csl;
    mx_print_json_object(js, "mx_process_input_from_server");
    sprintf(command, "INSERT INTO room_user (user_id, room_id, role) VALUES ('%d', '%d', '%d');", user_id, room_id, 0);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        json_object *room_data = mx_js_o_o_get(js, "room_data");
        json_object *messages = json_object_new_array();
        char *command1 = mx_strnew(1024);

        mx_js_o_o_add(room_data, "messages", messages);
        sprintf(command1, "SELECT *  FROM msg_history, users \
                where room_id = %d and users.id = msg_history.user_id order by msg_history.id desc limit 5;", room_id);
        if (sqlite3_exec(info->db, command1, mx_get_rooms_data, messages, 0) == SQLITE_OK) {
            mx_strdel(&command1);
            mx_send_json_to_all_in_room(info, js);
            mx_strdel(&command);
        }
    }
    return 1;
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

int mx_edit_profile (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
    int add_info = mx_js_g_int(mx_js_o_o_get(js, "add_info"));
    char command[1024];
    const char *json_str = NULL;

    if (!add_info) {
        const char *column = mx_js_g_str(mx_js_o_o_get(js, "column"));
        const char *data = mx_js_g_str(mx_js_o_o_get(js, "data"));

        sprintf(command, "UPDATE users SET %s='%s' where id='%d';", column, data, user_id);
    }
    else {
        int visual = mx_js_g_int(mx_js_o_o_get(js, "visual_n"));
        int audio = mx_js_g_int(mx_js_o_o_get(js, "audio_n"));
        int email = mx_js_g_int(mx_js_o_o_get(js, "email_n"));

        sprintf(command, "UPDATE user_notifications SET visual='%d', \
            audio='%d', email='%d' where user_id='%d';",
            visual, audio, email, user_id);
    }
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) 
        mx_js_o_o_add(js, "confirmation", mx_js_n_int(1));
    else 
        mx_js_o_o_add(js, "confirmation", mx_js_n_int(0));
    json_str = mx_js_o_to_js_str(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_str, strlen(json_str));
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

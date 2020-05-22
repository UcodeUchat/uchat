#include "uchat.h"

static int get_rooms_data(void *messages, int argc, char **argv, char **col_name) {
    json_object *message = json_object_new_object();
    json_object *id = json_object_new_int(atoi(argv[0]));
    json_object *user_id = json_object_new_int(atoi(argv[1]));
    json_object *data = NULL;
    json_object *login = json_object_new_string(argv[8]);
    json_object *add_info = NULL;

    if (argv[0] == NULL) {
        return 1;
    }
    (void)argc;
    (void)col_name;
    if (strcmp(argv[4], "file") == 0) {
        char *extention = strdup(argv[3]);

        while (mx_get_char_index(extention, '.') >= 0) {
            char *tmp = strdup(extention + mx_get_char_index(extention, '.') + 1);
            free(extention);
            extention = strdup(tmp);
            free(tmp); 
        }
        if (strcmp(extention, "jpg") == 0 || strcmp(extention, "png") == 0 || strcmp(extention, "gif") == 0) {
            data = json_object_new_string(argv[3]);
            add_info = json_object_new_int(2);
        }
        else {
            data = json_object_new_string(argv[3] /*+ 20*/);
            add_info = json_object_new_int(1);
        }
    }
    else {
        if (strcmp(argv[4], "mes") == 0)
            add_info = json_object_new_int(0);
        else if (strcmp(argv[4], "stik") == 0)
            add_info = json_object_new_int(3);
        data = json_object_new_string(argv[3]);
    }
    json_object_object_add(message, "id", id);
    json_object_object_add(message, "user_id", user_id);
    json_object_object_add(message, "data", data);
    json_object_object_add(message, "login", login);
    json_object_object_add(message, "add_info", add_info);
    json_object_array_add((struct json_object *)messages, message);
    return 0;
}

int mx_delete_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    (void)csl;
    int message_id = json_object_get_int(json_object_object_get(js, "message_id"));
    char *command = malloc(1024);

    sprintf(command, "DELETE FROM msg_history where id = %d;", message_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}

static int load_user_data(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    json_object *name = NULL;
    json_object *email = NULL;
    json_object *login = json_object_new_string(argv[2]);
    if (argv[4])
        name = json_object_new_string(argv[4]);
    else
        name = json_object_new_string("");
    if (argv[5])
        email = json_object_new_string(argv[5]);
    else
        email = json_object_new_string("");
    json_object *visual_n = json_object_new_int(atoi(argv[9]));
    json_object *audio_n = json_object_new_int(atoi(argv[10]));
    json_object *email_n = json_object_new_int(atoi(argv[11]));

    json_object_object_add((struct json_object *)js, "login", login);
    json_object_object_add((struct json_object *)js, "name", name);
    json_object_object_add((struct json_object *)js, "email", email);
    json_object_object_add((struct json_object *)js, "visual_n", visual_n);
    json_object_object_add((struct json_object *)js, "audio_n", audio_n);
    json_object_object_add((struct json_object *)js, "email_n", email_n);
    return 0;
}

int mx_leave_room (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    char *command = malloc(1024);
    const char *json_string = NULL;

    sprintf(command, "DELETE FROM room_user where user_id='%d' and room_id='%d';", user_id, room_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
        json_string = json_object_to_json_string(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}

int mx_join_room (t_server_info *info, t_socket_list *csl, json_object *js) {
    (void)csl;
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    char *command = malloc(1024);
    mx_print_json_object(js, "mx_process_input_from_server");

    sprintf(command, "INSERT INTO room_user (user_id, room_id, role) VALUES ('%d', '%d', '%d');", user_id, room_id, 0);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        printf("1\n");
        json_object *room_data = json_object_object_get(js, "room_data");
        mx_print_json_object(room_data, "mx_process_input_from_server");
        printf("2\n");
        json_object *messages = json_object_new_array();
        char *command1 = mx_strnew(1024);

        json_object_object_add(room_data, "messages", messages);
        printf("3\n");
        sprintf(command1, "SELECT *  FROM msg_history, users \
                where room_id = %d and users.id = msg_history.user_id order by msg_history.id desc limit 5;", room_id);
        if (sqlite3_exec(info->db, command1, mx_get_rooms_data, messages, 0) == SQLITE_OK) {
            mx_strdel(&command1);
            mx_send_json_to_all_in_room(info, js);
            mx_strdel(&command);
        }
        else {
            printf("fail2\n");
        }
    }
    else {
        printf("fail\n");
    }
    return 1;
}

static int search_rooms(void *array, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        json_object *room = json_object_new_object();
        json_object *id = json_object_new_int(atoi(argv[0]));
        json_object *name = json_object_new_string(argv[1]);
        json_object *acces = json_object_new_int(atoi(argv[2]));

        json_object_object_add(room, "id", id);
        json_object_object_add(room, "name", name);
        json_object_object_add(room, "acces", acces);
        json_object_array_add((struct json_object *)array, room);
    }
    return 0;
}

static int search_users(void *array, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        json_object *user = json_object_new_object();
        json_object *id = json_object_new_int(atoi(argv[0]));
        json_object *login = json_object_new_string(argv[2]);
        // json_object *email = json_object_new_string(argv[5]);
        // json_object *name = json_object_new_string(argv[4]);

        json_object_object_add(user, "id", id);
        // json_object_object_add(user, "name", name);
        json_object_object_add(user, "login", login);
        // json_object_object_add(user, "email", email);
        json_object_array_add((struct json_object *)array, user);
    }
    return 0;
}

int mx_search_all (t_server_info *info, t_socket_list *csl, json_object *js) {
    const char *query = json_object_get_string(json_object_object_get(js, "query"));
    char *command = malloc(1024);
    char *command1 = malloc(1024);
    const char *json_string = NULL;
    json_object *array_rooms = json_object_new_array();
    json_object *array_users = json_object_new_array();

    json_object_object_add(js, "rooms", array_rooms);
    json_object_object_add(js, "users", array_users);
    if (strcmp(query, "All") == 0) {
        sprintf(command, "SELECT * FROM rooms;");
        sprintf(command1, "SELECT * FROM users;");
    }
    else {
        sprintf(command, "SELECT * FROM rooms WHERE name LIKE '%%%s%%';", query);
        sprintf(command1, "SELECT * FROM users WHERE login LIKE '%%%s%%';", query);
    }
    if (sqlite3_exec(info->db, command, search_rooms, array_rooms, NULL) != SQLITE_OK) {
        printf("fail\n");
        return 0;
    }
    if (sqlite3_exec(info->db, command1, search_users, array_users, NULL) != SQLITE_OK) {
        printf("fail\n");
        return 0;
    }

    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    mx_strdel(&command);
    return 1;
}

int mx_load_profile (t_server_info *info, t_socket_list *csl, json_object *js) {
    int id = json_object_get_int(json_object_object_get(js, "id"));
    char *command = malloc(1024);
    const char *json_string = NULL;

    sprintf(command, "SELECT * FROM users, user_notifications where users.id='%d' and user_notifications.user_id='%d';", id, id);
    if (sqlite3_exec(info->db, command, load_user_data, js, NULL) == SQLITE_OK) {
        json_string = json_object_to_json_string(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}

int mx_edit_profile (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int add_info = json_object_get_int(json_object_object_get(js, "add_info"));
    char *command = malloc(1024);
    const char *json_string = NULL;

    if (!add_info) {
        const char *column = json_object_get_string(json_object_object_get(js, "column"));
        const char *data = json_object_get_string(json_object_object_get(js, "data"));
        sprintf(command, "UPDATE users SET %s='%s' where id='%d';", column, data, user_id);
    }
    else {
        int visual = json_object_get_int(json_object_object_get(js, "visual_n"));
        int audio = json_object_get_int(json_object_object_get(js, "audio_n"));
        int email = json_object_get_int(json_object_object_get(js, "email_n"));
        sprintf(command, "UPDATE user_notifications SET visual='%d', \
            audio='%d', email='%d' where user_id='%d';", visual, audio, email, user_id);
    }
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) 
        json_object_object_add(js, "confirmation", json_object_new_int(1));
    else 
        json_object_object_add(js, "confirmation", json_object_new_int(0));
    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    mx_strdel(&command);
    return 1;
}


int mx_edit_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    (void)csl;
    int message_id = json_object_get_int(json_object_object_get(js, "message_id"));
    const char *data = json_object_get_string(json_object_object_get(js, "data"));
    char *command = malloc(1024);

    sprintf(command, "UPDATE msg_history SET message='%s' where id='%d';", data, message_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}


int mx_load_history (t_server_info *info, t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    int last_id = json_object_get_int(json_object_object_get(js, "last_id"));
    char *command = malloc(1024);
    json_object *messages = json_object_new_array();

    json_object_object_add(js, "messages", messages);
    sprintf(command, "SELECT *  FROM msg_history, users \
                where room_id = %d and msg_history.id < %d and users.id = msg_history.user_id \
                order by msg_history.id desc limit 15;", room_id, last_id);
    if (sqlite3_exec(info->db, command, get_rooms_data, messages, NULL) == SQLITE_OK) {
        printf("succes\n");
    }
    else {
        printf("fail\n");
    }
    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    mx_strdel(&command);
    return 1;
}

int mx_logout (t_server_info *i, t_socket_list *csl, json_object *js) {
	(void)csl;
    //const char *json_string = NULL;
    char *command = malloc(1024);
    int id = json_object_get_int(json_object_object_get(js, "user_id"));

    sprintf(command, "UPDATE users SET socket='0' WHERE id='%s'", mx_itoa(id));
    if (sqlite3_exec(i->db, command, NULL, NULL, NULL) == SQLITE_OK) {
		printf("succes\n");
    }
    else {
    	printf("fail\n");
    }
    mx_strdel(&command);
	return 1;
}

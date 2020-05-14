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
    if (strcmp(argv[4], "file") == 0){
        data = json_object_new_string(argv[3] + 20);
        add_info = json_object_new_int(1);
    }
    else {
        data = json_object_new_string(argv[3]);
        add_info = json_object_new_int(0);
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
    //const char *json_string = NULL;
    (void)csl;
    int message_id = json_object_get_int(json_object_object_get(js, "message_id"));
    char *command = malloc(1024);

    sprintf(command, "DELETE FROM msg_history where id = %d ", message_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        //json_string = json_object_to_json_string(js);
        mx_send_json_to_all_in_room(info, js);
        //mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
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

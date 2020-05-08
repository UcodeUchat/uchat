#include "uchat.h"

static int print_time(void *point, int argc, char **argv, char **col_name) {
    (void)point;
    (void)argc;
    (void)col_name;
    printf("Time: %s\n", argv[0]);
    return 0;
}


int save_message(t_server_info *info, json_object *js) {
	char *command = malloc(1024);
    char *command2 = malloc(1024);
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    const char *message = json_object_get_string(json_object_object_get(js, "data"));

    sprintf(command, "INSERT INTO msg_history (user_id, room_id, message, \
                addition_cont) VALUES ('%d', '%d', '%s', 'mes'); SELECT id from msg_history where ",
                user_id, room_id, message);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) != SQLITE_OK) {
		return -1;
    }
    mx_strdel(&command);

    sprintf(command2, "SELECT time from msg_history;");
    if (sqlite3_exec(info->db, command2, print_time, NULL, NULL) != SQLITE_OK) {
        return -1;
    }
    mx_strdel(&command2);
	return 1;
}

int mx_process_message_in_server(t_server_info *info, json_object *js) {
    int res  = save_message(info, js);
    if (res > 0)
    	mx_send_json_to_all_in_room(info, js);
    else
    	printf("loh, ne sohranilos'\n");
    return 0;
}

// id|user_id|room_id|message|addition_cont|time
// 0|0|0|hello|mes|

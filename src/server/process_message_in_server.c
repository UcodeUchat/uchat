#include "uchat.h"

int mx_get_data(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        struct json_object *t = json_object_new_int(atoi(argv[0]));
        json_object_object_add((struct json_object*) js, "id", t);
        return 0;
    }
    return 1;
}

// static int print_time(void *point, int argc, char **argv, char **col_name) {
//     (void)point;
//     (void)argc;
//     (void)col_name;
//     printf("Time: %s\n", argv[0]);
//     return 0;
// }

int save_message(t_server_info *info, json_object *js) {
	char command[1024];
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    char *message = mx_replace_substr(json_object_get_string(json_object_object_get(js, "data")), "'", "''");

    command[sprintf(command, "INSERT INTO msg_history (user_id, room_id, message, addition_cont)\
     		VALUES ('%d', '%d', '%s', 'mes'); SELECT last_insert_rowid()", 
            user_id, room_id, message)] = '\0';
    mx_strdel(&message);
    if (sqlite3_exec(info->db, command, mx_get_data, js, NULL) != SQLITE_OK) {
		return -1;
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

// id|user_id|room_id|message|addition_cont|time
// 0|0|0|hello|mes|

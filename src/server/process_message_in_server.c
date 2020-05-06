#include "uchat.h"

int save_message(t_server_info *info, json_object *js) {
	char *command = malloc(1024);
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    const char *message = json_object_get_string(json_object_object_get(js, "data"));

    sprintf(command, "INSERT INTO msg_history (user_id, room_id, message, addition_cont)\
     		VALUES ('%d', '%d', '%s', 'mes') ", user_id, room_id, message);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) != SQLITE_OK) {
		return -1;
    }
    mx_strdel(&command);
	return 1;
}

int mx_process_message_in_server(t_server_info *info, json_object *js) {
    // save to db before sending
    int res  = save_message(info, js);
    if (res > 0)
    	mx_send_json_to_all_in_room(info, js);
    else
    	printf("loh, ne sohranilos'\n");
    return 0;
}

// id|user_id|room_id|message|addition_cont|time
// 0|0|0|hello|mes|
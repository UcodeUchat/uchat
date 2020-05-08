#include "uchat.h"

static int get_data(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        struct json_object *t = json_object_new_int(atoi(argv[0]));
        json_object_object_add((struct json_object*) js, "id", t);
        return 0;
    }
    return 1;
}

int save_message(t_server_info *info, json_object *js) {
	char *command = malloc(1024);
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    const char *message = json_object_get_string(json_object_object_get(js, "data"));
    time_t lt= time(NULL);
    
    sprintf(command, "INSERT INTO msg_history (user_id, room_id, message, addition_cont, time)\
     		VALUES ('%d', '%d', '%s', 'mes', '%s'); SELECT last_insert_rowid()", 
            user_id, room_id, message, ctime(&lt));
    if (sqlite3_exec(info->db, command, get_data, js, NULL) != SQLITE_OK) {
		return -1;
    }
    mx_strdel(&command);
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

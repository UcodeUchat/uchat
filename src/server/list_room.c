#include "uchat.h"

static int get_rooms(void *array, int argc, char **argv, char **col_name) {
	json_object *room = json_object_new_object();
	json_object *id = json_object_new_int(atoi(argv[1]));
	json_object *name = json_object_new_string(argv[2]);
	json_object *access = json_object_new_int(atoi(argv[3]));

	(void)argc;
	(void)col_name;
	json_object_object_add(room, "room_id", id);
	json_object_object_add(room, "name", name);
	json_object_object_add(room, "access", access);
	json_object_array_add((struct json_object *)array, room);
	return 0;
}

int mx_get_rooms_data(void *messages, int argc, char **argv, char **col_name) {
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
	    else if (strcmp(extention, "aif") == 0) {
            data = json_object_new_string(argv[3]);
            add_info = json_object_new_int(4);
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

void mx_get_rooms(t_server_info *info, json_object *js) {
	char *req = mx_strnew(1024);
	int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
	json_object *array = json_object_new_array();

	json_object_object_add(js, KEY10, array);
    sprintf(req, "select distinct user_id, rooms.id, rooms.name, rooms.access from room_user, rooms \
    			where rooms.id=room_user.room_id and user_id=%d;", user_id);
    printf("%s\n", req);
    if (sqlite3_exec(info->db, req, get_rooms, array, 0) != SQLITE_OK) {
        printf("Work rooms\n");
        return;
    }
    int n_rooms = json_object_array_length(array);
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(array, i);
        json_object *messages = json_object_new_array();
        int room_id = json_object_get_int(json_object_object_get(room_data, "room_id"));
        char *req1 = mx_strnew(1024);

        json_object_object_add(room_data, "messages", messages);
        sprintf(req1, "SELECT *  FROM msg_history, users \
        		where room_id = %d and users.id = msg_history.user_id order by msg_history.id desc limit 5;", room_id);
        if (sqlite3_exec(info->db, req1, mx_get_rooms_data, messages, 0) != SQLITE_OK) {
        	printf("Work rooms data\n");
    	}
    	mx_strdel(&req1);
    }
    mx_print_json_object(js, "js_in_room");
    mx_strdel(&req);
}

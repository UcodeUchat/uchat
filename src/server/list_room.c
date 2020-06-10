#include "uchat.h"

static int get_rooms(void *array, int argc, char **argv, char **col_name) {
	json_object *room = mx_js_n_o();
	json_object *id = mx_js_n_int(atoi(argv[1]));
	json_object *name = mx_js_n_str(argv[2]);
	json_object *access = mx_js_n_int(atoi(argv[3]));

	(void)argc;
	(void)col_name;
	mx_js_o_o_add(room, "room_id", id);
	mx_js_o_o_add(room, "name", name);
	mx_js_o_o_add(room, "access", access);
	json_object_array_add((struct json_object *)array, room);
	return 0;
}

static void rooms_cycle(t_server_info *info, json_object **array, int n_rooms) {
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(*array, i);
        json_object *messages = json_object_new_array();
        int room_id = mx_js_g_int(mx_js_o_o_get(room_data, "room_id"));
        char *req1 = mx_strnew(1024);

        mx_js_o_o_add(room_data, "messages", messages);
        sprintf(req1, "SELECT *  FROM msg_history, users \
        		where room_id = %d and users.id = msg_history.user_id order \
				by msg_history.id desc limit 5;", room_id);
        if (sqlite3_exec(info->db, req1, mx_get_rooms_data, messages, 0)
			!= SQLITE_OK) {
        	printf("Work rooms data\n");
    	}
    	mx_strdel(&req1);
    }
}

void mx_get_rooms(t_server_info *info, json_object *js) {
	char *req = mx_strnew(1024);
	int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
	json_object *array = json_object_new_array();
	int n_rooms;

	mx_js_o_o_add(js, KEY10, array);
    sprintf(req, "select distinct room_user.user_id, rooms.id, rooms.name, \
			rooms.access from room_user, rooms \
    			where rooms.id=room_user.room_id and user_id=%d;", user_id);
    if (sqlite3_exec(info->db, req, get_rooms, array, 0) != SQLITE_OK)
        return;
    n_rooms = json_object_array_length(array);
	rooms_cycle(info, &array, n_rooms);
    mx_print_json_object(js, "js_in_room");
    mx_strdel(&req);
}

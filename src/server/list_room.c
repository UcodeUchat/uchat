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

static void set_file(json_object **add_info, json_object **data, char **argv) {
	char *ext = strdup(argv[3]);

	while (mx_get_char_index(ext, '.') >= 0) {
		mx_strdel(&ext);
		ext = strdup(argv[3] + mx_get_char_index(argv[3], '.') + 1);
	}
	if (strcmp(ext, "jpg") == 0 || strcmp(ext, "png") == 0
		|| strcmp(ext, "gif") == 0) {
		*data = mx_js_n_str(argv[3]);
		*add_info = mx_js_n_int(2);
	}
	else if (strcmp(ext, "aif") == 0) {
		*data = mx_js_n_str(argv[3]);
		*add_info = mx_js_n_int(4);
	}
	else {
		*data = mx_js_n_str(argv[3] /*+ 20*/);
		*add_info = mx_js_n_int(1);
	}
}

static void set_msg(json_object **add_info, json_object **data, char **argv) {
	if (strcmp(argv[4], "mes") == 0)
		*add_info = mx_js_n_int(0);
	else if (strcmp(argv[4], "stik") == 0)
		*add_info = mx_js_n_int(3);
	*data = mx_js_n_str(argv[3]);
}

static void set_data_to_room_json(json_object **message, json_object **id,
						   json_object **user_id, json_object **data) {
	mx_js_o_o_add(*message, "id", *id);
	mx_js_o_o_add(*message, "user_id", *user_id);
	mx_js_o_o_add(*message, "data", *data);
}

static void set_data_to_room_json_2(json_object **message, json_object **login,
							 json_object **add_info, void **messages) {
	mx_js_o_o_add(*message, "login", *login);
	mx_js_o_o_add(*message, "add_info", *add_info);
	json_object_array_add(*(struct json_object **)messages, *message);
}

int mx_get_rooms_data(void *messages, int argc, char **argv, char **col_name) {
	json_object *message = mx_js_n_o();
	json_object *id = mx_js_n_int(atoi(argv[0]));
	json_object *user_id = mx_js_n_int(atoi(argv[1]));
	json_object *data = NULL;
	json_object *login = mx_js_n_str(argv[8]);
	json_object *add_info = NULL;

	if (argv[0] == NULL)
		return 1;
	(void)argc;
	(void)col_name;
	if (strcmp(argv[4], "file") == 0)
	    set_file(&add_info, &data, argv);
	else {
		set_msg(&add_info, &data, argv);
	}
	set_data_to_room_json(&message, &id, &user_id, &data);
	set_data_to_room_json_2(&message, &login, &add_info, &messages);
	return 0;
}

void mx_get_rooms(t_server_info *info, json_object *js) {
	char *req = mx_strnew(1024);
	int user_id = mx_js_g_int(mx_js_o_o_get(js, "user_id"));
	json_object *array = json_object_new_array();
	int n_rooms;

	mx_js_o_o_add(js, KEY10, array);
    sprintf(req, "select distinct user_id, rooms.id, rooms.name, rooms.access from room_user, rooms \
    			where rooms.id=room_user.room_id and user_id=%d;", user_id);
    if (sqlite3_exec(info->db, req, get_rooms, array, 0) != SQLITE_OK)
        return;
    n_rooms = json_object_array_length(array);
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(array, i);
        json_object *messages = json_object_new_array();
        int room_id = mx_js_g_int(mx_js_o_o_get(room_data, "room_id"));
        char *req1 = mx_strnew(1024);

        mx_js_o_o_add(room_data, "messages", messages);
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

#include "uchat.h"

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

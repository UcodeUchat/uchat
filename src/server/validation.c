#include "uchat.h"

int msg_valigation (json_object *js) {
	json_object *login = mx_js_o_o_get(js, "login");
	json_object *data = mx_js_o_o_get(js, "data");
	json_object *user_id = mx_js_o_o_get(js, "user_id");
	json_object *room_id = mx_js_o_o_get(js, "room_id");
	json_object *add_info = mx_js_o_o_get(js, "add_info");

	if (!login)
		return 1;
	if (!data)
		return 1;
	if (!user_id)
		return 1;
	if (!room_id)
		return 1;
	if (!add_info)
		return 1;
	return 0;
}

int mx_validation(json_object *js) {
	json_object *js_type = mx_js_o_o_get(js, "type");
	int js1_type;
	int type;

	if (js_type != NULL) {
		js1_type = json_object_get_type(js_type);
		if (js1_type == 3) {
			type = json_object_get_int(js_type);
			if (type == MX_MSG_TYPE)
				return msg_valigation(js);
			else return 0;
		}
		else {
			printf("type != int\n");
			return 1;
		}
	}
	else { 
		printf("not exists type\n");
		return 1;
	}
}

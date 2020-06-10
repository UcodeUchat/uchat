#include "uchat.h"

int mx_msg_validation (json_object *js) { // 1
	json_object *login = mx_js_o_o_get(js, "login");
	json_object *data = mx_js_o_o_get(js, "data");
	json_object *user_id = mx_js_o_o_get(js, "user_id");
	json_object *room_id = mx_js_o_o_get(js, "room_id");
	json_object *add_info = mx_js_o_o_get(js, "add_info");

	if (!login || !data || !user_id || !room_id || !add_info)
		return 1;
	return MX_OK;
}

int mx_file_send_validation(json_object *js) { // 2
	json_object *piece = mx_js_o_o_get(js, "piece");
	json_object *user_id = mx_js_o_o_get(js, "user_id");

	if (piece && user_id) {
		int piece_id = mx_js_g_int(piece);

		if (piece_id == 1) {
			json_object *file_name = mx_js_o_o_get(js, "file_name");
			json_object *file_size = mx_js_o_o_get(js, "file_size");

			return !file_name || !file_size || !user_id ? 1 : 0;
		}
		else if (piece_id == 2) {
			json_object *data = mx_js_o_o_get(js, "data");

			return data ? 0 : 1;
		}
		return MX_OK;
	}
	return 1;
}

int mx_auth_validation(json_object *js) { // 3
	json_object *login = mx_js_o_o_get(js, "login");
	json_object *password = mx_js_o_o_get(js, "password");

	if (!login || !password)
		return 1;
	return MX_OK;
}

int mx_logout_validation(json_object *js) { // 9
	json_object *user_id = mx_js_o_o_get(js, "user_id");

	if (!user_id)
		return 1;
	return MX_OK;
}

int mx_load_more_validation(json_object *js) { // 10
	json_object *room_id = mx_js_o_o_get(js, "room_id");
	json_object *last_id = mx_js_o_o_get(js, "last_id");

	if (!room_id || !last_id)
		return 1;
	return MX_OK;
}

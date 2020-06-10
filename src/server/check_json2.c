#include "uchat.h"

int mx_delete_msg_validation(json_object *js) { // 11
	json_object *message_id = mx_js_o_o_get(js, "message_id");

	if (!message_id)
		return 1;
	return MX_OK;
}

int mx_file_download_validation(json_object *js) { // 12
	json_object *room_id = mx_js_o_o_get(js, "room_id");
	json_object *user_id = mx_js_o_o_get(js, "user_id");
	json_object *file_id = mx_js_o_o_get(js, "file_id");

	if (!room_id || !user_id || !file_id)
		return 1;
	return MX_OK;
}

int mx_edit_message_validation(json_object *js) { // 13
	json_object *message_id = mx_js_o_o_get(js, "message_id");
	json_object *data = mx_js_o_o_get(js, "data");

	if (!message_id || !data)
		return 1;
	return MX_OK;
}

int mx_load_profile_validation(json_object *js) { // 14
	json_object *id = mx_js_o_o_get(js, "id");

	if (!id)
		return 1;
	return MX_OK;
}

int mx_edit_profile_validation(json_object *js) { // 15
	json_object *user_id = mx_js_o_o_get(js, "user_id");
	json_object *add_info = mx_js_o_o_get(js, "add_info");
	int add = mx_js_g_int(mx_js_o_o_get(js, "add_info"));

	if (user_id && add_info) {
		if (!add) {
			json_object *column = mx_js_o_o_get(js, "column");
			json_object *data = mx_js_o_o_get(js, "data");

			return !column || !data ? 1 : 0;
		}
		else {
			json_object *visual_n = mx_js_o_o_get(js, "visual_n");
			json_object *audio_n = mx_js_o_o_get(js, "audio_n");
			json_object *email_n = mx_js_o_o_get(js, "email_n");

			return !visual_n || !audio_n || !email_n ? 1 : 0;
		}
	}
	return 1;
}

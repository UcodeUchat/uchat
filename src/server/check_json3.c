#include "uchat.h"

int mx_leave_room_validation(json_object *js) { // 16
	json_object *user_id = mx_js_o_o_get(js, "user_id");
	json_object *room_id = mx_js_o_o_get(js, "room_id");

	if (!user_id || !room_id)
		return 1;
	return MX_OK;
}

int mx_search_all_validation(json_object *js) { // 17
	json_object *query = mx_js_o_o_get(js, "query");

	if (!query)
		return 1;
	return MX_OK;
}

int mx_join_room_validation(json_object *js) { // 18
	json_object *user_id = mx_js_o_o_get(js, "user_id");
	json_object *room_id = mx_js_o_o_get(js, "room_id");

	if (!user_id || !room_id)
		return 1;
	return MX_OK;
}

int mx_create_room_validation(json_object *js) { // 19
	json_object *user_id = mx_js_o_o_get(js, "user_id");
	json_object *acces = mx_js_o_o_get(js, "acces");
	json_object *name = mx_js_o_o_get(js, "name");
	json_object *room_data = mx_js_o_o_get(js, "room_data");

	if (!user_id || !acces || !name || !room_data )
		return 1;
	return MX_OK;
}

int mx_direct_message_validation(json_object *js) { // 20
	json_object *first_id = mx_js_o_o_get(js, "first_id");
	json_object *second_id = mx_js_o_o_get(js, "second_id");

	if (!first_id || !second_id)
		return 1;
	return MX_OK;
}

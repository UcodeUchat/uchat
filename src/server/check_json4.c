#include "uchat.h"

int mx_delete_acc_validation(json_object *js) { // 21
	json_object *user_id = mx_js_o_o_get(js, "user_id");

	if (!user_id)
		return 1;
	return MX_OK;
}

int mx_reconnection_validation(json_object *js) { // 22
	json_object *login = mx_js_o_o_get(js, "login");
	json_object *password = mx_js_o_o_get(js, "password");

	if (!login || !password)
		return 1;
	return MX_OK;
}

int mx_reg_validation(json_object *js) { // 6
	json_object *login = mx_js_o_o_get(js, "login");
	json_object *password = mx_js_o_o_get(js, "password");

	if (!login || !password)
		return 1;
	return MX_OK;
}

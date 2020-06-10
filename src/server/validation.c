#include "uchat.h"

int validation_4(json_object *js, int type) {
	if (type == MX_REG_TYPE)
		return mx_reg_validation(js);
	else if (type >= 4 && type <= 8)
		return MX_OK;
	else
		return 1;
}

int validation_3(json_object *js, int type) {
	if (type == MX_EDIT_PROFILE_TYPE)
		return mx_edit_profile_validation(js);
	else if (type == MX_LEAVE_ROOM_TYPE)
		return mx_leave_room_validation(js);
	else if (type == MX_SEARCH_ALL_TYPE)
		return mx_search_all_validation(js);
	else if (type == MX_JOIN_ROOM_TYPE)
		return mx_join_room_validation(js);
	else if (type == MX_CREATE_ROOM_TYPE)
		return mx_create_room_validation(js);
	else if (type == MX_DIRECT_MESSAGE_TYPE)
		return mx_direct_message_validation(js);
	else if (type == MX_DELETE_ACCOUNT_TYPE)
		return mx_delete_acc_validation(js);
	else if (type == MX_RECONNECTION_TYPE)
		return mx_reconnection_validation(js);
	else if (type == MX_EMPTY_JSON)
		return 0;
	else
		return validation_4(js, type);
}

int validation_2(json_object *js, int type) {
	if (type == MX_MSG_TYPE)
		return mx_msg_validation(js);
	else if (type == MX_FILE_SEND_TYPE)
		return mx_file_send_validation(js);
	else if (type == MX_AUTH_TYPE)
		return mx_auth_validation(js);
	else if (type == MX_LOGOUT_TYPE)
		return mx_logout_validation(js);
	else if (type == MX_LOAD_MORE_TYPE)
		return mx_load_more_validation(js);
	else if (type == MX_DELETE_MESSAGE_TYPE)
		return mx_delete_msg_validation(js);
	else if (type == MX_FILE_DOWNLOAD_TYPE)
		return mx_file_download_validation(js);
	else if (type == MX_EDIT_MESSAGE_TYPE)
		return mx_edit_message_validation(js);
	else if (type == MX_LOAD_PROFILE_TYPE)
		return mx_load_profile_validation(js);
	else
		return validation_3(js, type);
}

int mx_validation(json_object *js) {
	json_object *js_type = mx_js_o_o_get(js, "type");
	int js1_type;
	int type;

	if (js_type != NULL) {
		js1_type = json_object_get_type(js_type);
		if (js1_type == 3) {
			type = json_object_get_int(js_type);
			//printf("type = %d\n", type);
			return validation_2(js, type);
		}
		else
			return 1;
	}
	else { 
		printf("not exists type\n");
		return 1;
	}
}

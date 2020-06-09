#include "uchat.h"

void mx_send_json_to_all_in_room(t_server_info *info, json_object *json_obj) {
    int *sockets_online = mx_get_users_sock_in_room(&info,
        mx_js_g_int(mx_js_o_o_get(json_obj,
        "room_id")));
    t_socket_list *tls_list = NULL;
    const char *json_string = mx_js_o_to_js_str(json_obj);

    for (int i = 0; i < info->wdb->i; i++) {
        tls_list = mx_find_socket_elem(info->socket_list, sockets_online[i]);
        if (tls_list) {
            mx_save_send(&tls_list->mutex, tls_list->tls_socket, json_string,
                strlen(json_string));
        }
    }
}

static void run3(t_server_info *info, t_socket_list *csl, int* return_value,
                 int type) {
    if (type == MX_DELETE_ACCOUNT_TYPE)
        *return_value = mx_delete_acc(info, csl->obj);
    else if (type == MX_RECONNECTION_TYPE)
        *return_value = mx_reconnection(info, csl);
}

static void run2(t_server_info *info, t_socket_list *csl, int* return_value,
                 int type) {
    if (type == MX_DELETE_MESSAGE_TYPE)
        *return_value = mx_delete_message(info, csl, csl->obj);
    else if (type == MX_EDIT_MESSAGE_TYPE)
        *return_value = mx_edit_message(info, csl, csl->obj);
    else if (type == MX_LOAD_PROFILE_TYPE)
        *return_value = mx_load_profile(info, csl, csl->obj);
    else if (type == MX_EDIT_PROFILE_TYPE)
        *return_value = mx_edit_profile(info, csl, csl->obj);
    else if (type == MX_LEAVE_ROOM_TYPE)
        *return_value = mx_leave_room(info, csl, csl->obj);
    else if (type == MX_SEARCH_ALL_TYPE)
        *return_value = mx_search_all(info, csl, csl->obj);
    else if (type == MX_JOIN_ROOM_TYPE)
        *return_value = mx_join_room(info, csl, csl->obj);
    else if (type == MX_CREATE_ROOM_TYPE)
        *return_value = mx_create_room_server(info, csl, csl->obj);
    else if (type == MX_DIRECT_MESSAGE_TYPE)
        *return_value = mx_direct_message(info, csl, csl->obj);
    else
        run3(info, csl, return_value, type);
}

int mx_run_function_type(t_server_info *info, t_socket_list *csl) {
    int validation = mx_validation(csl->obj);
    if (!validation) {
        int return_value = -1;
        int type = mx_js_g_int(mx_js_o_o_get(csl->obj, "type"));

        if (type == MX_MSG_TYPE)
            return mx_process_message_in_server(info, csl->obj);
        else if (type == MX_FILE_SEND_TYPE)
            return mx_save_file_in_server(info, csl);
        else if (type == MX_FILE_DOWNLOAD_TYPE)
            return mx_send_file_from_server(info, csl);
        else if (type == MX_AUTH_TYPE)
        	return mx_authorization(info, csl, csl->obj);
        else if (type == MX_REG_TYPE)
            return mx_registration(info, csl, csl->obj);
        else if (type == MX_LOGOUT_TYPE)
            return mx_logout(info, csl, csl->obj);
        else if (type == MX_LOAD_MORE_TYPE)
            return mx_load_history(info, csl, csl->obj);
        else
            run2(info, csl, &return_value, type);
        return type == MX_EMPTY_JSON ? 0 : return_value;
    }
    else {
        printf("not_valid\n");
        return 1;
    }
}

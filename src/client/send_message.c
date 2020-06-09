#include "uchat.h"

void mx_process_message_in_client(t_client_info *info) {
    char *message = NULL;
    const char *json_string;
    json_object *obj = mx_create_basic_json_object(MX_MSG_TYPE);

    mx_js_o_o_add(obj, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(obj, "room_id", mx_js_n_int(info->data->current_room));
    mx_js_o_o_add(obj, "login", mx_js_n_str(info->login));
    mx_js_o_o_add(obj, "add_info", mx_js_n_int(0));
    message = strdup((char *)gtk_entry_get_text(GTK_ENTRY
                                            (info->data->message_entry)));
    mx_js_o_o_add(obj, "data", mx_js_n_str(message));
    json_string = mx_js_o_to_js_str(obj);
    tls_send(info->tls_client, json_string, strlen(json_string));
    mx_strdel(&message);
    json_object_put(obj);
}

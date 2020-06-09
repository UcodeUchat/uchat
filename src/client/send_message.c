#include "uchat.h"

void mx_process_message_in_client(t_client_info *info) {
    char *message = NULL;
    const char *json_string;
    json_object *obj = mx_create_basic_json_object(MX_MSG_TYPE);

    json_object_object_add(obj, "user_id", json_object_new_int(info->id));
    json_object_object_add(obj, "room_id", json_object_new_int(info->data->current_room));
    json_object_object_add(obj, "login", json_object_new_string(info->login));
    json_object_object_add(obj, "password", json_object_new_string(info->password));
    json_object_object_add(obj, "add_info", json_object_new_int(0));

    message = strdup((char *)gtk_entry_get_text(GTK_ENTRY(info->data->message_entry)));
    json_object_object_add(obj, "data", json_object_new_string(message));
    json_string = json_object_to_json_string(obj);
    tls_send(info->tls_client, json_string, strlen(json_string));
    mx_strdel(&message);
    json_object_put(obj);
}

#include "uchat.h"

int mx_send_message_from_client(t_client_info *info, t_package *package, char *message) {
    int msg_size = strlen(message);

    if (msg_size > MX_MAX_DATA_SIZE) {
        printf("send more than 1 piece\n"); // #
        int pos_in_data = 0;

        package->piece = 1;
        strncat(package->data, message, MX_MAX_DATA_SIZE);
        json_object *new_json = mx_package_to_json(package);
        mx_print_json_object(new_json, "mx_send_message_from_client");

        tls_write(info->tls_client, package, MX_PACKAGE_SIZE);
        while ((pos_in_data += MX_MAX_DATA_SIZE) < msg_size) {
            package->piece = pos_in_data + MX_MAX_DATA_SIZE < msg_size ? 2 : 3;
            mx_memset(package->data, 0, MX_MAX_DATA_SIZE);
            strncat(package->data, message + pos_in_data, MX_MAX_DATA_SIZE);
            tls_write(info->tls_client, package, MX_PACKAGE_SIZE);
        }
    }
    else {
        printf("send 1 piece\n"); // #
        package->piece = 0;
        strncat(package->data, message, MX_MAX_DATA_SIZE);

    ////****
        json_object *new_json = mx_package_to_json(package);
//        mx_print_json_object(new_json, "mx_send_message_from_client");
        const char *json_string = json_object_to_json_string(new_json);
        tls_send(info->tls_client, json_string, strlen(json_string));
    ////****
//        tls_send(info->tls_client, package, MX_PACKAGE_SIZE);
    }
    return 0;
}

void mx_process_message_in_client(t_client_info *info) {
    char *message = NULL;
    t_package *package = mx_create_new_package();
    // tmp
    package->user_id = info->id;
    package->room_id = info->data->current_room;
    strncat(package->login, info->login, sizeof(package->login) - 1);
    strncat(package->password, info->password, sizeof(package->password) - 1);
    //
    package->type = MX_MSG_TYPE;
    message = strdup((char *)gtk_entry_get_text(GTK_ENTRY(info->data->message_entry)));
    mx_send_message_from_client(info, package, message);
    mx_strdel(&message);
    mx_memset(package->data, 0, sizeof(package->data));
    // tmp
    printf("back to menu\n");
    //break;
        // mx_get_input(package->data);
        // printf("sizeof(package) = %zu\n", sizeof(package));
        // write(info->socket, package, MX_PACKAGE_SIZE);
        //
    free(package);
}

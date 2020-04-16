#include "uchat.h"

// char *get_message() {
//     printf("get_message\n"); // #
//     char *message = malloc(1024);
//     int pos = 0;
//     int size = 1024;

//     while (1) {
//         if (fgets(message + pos, MAX_CLIENT_INPUT, stdin) != NULL) {
//         pos += strlen(message + pos);
//         if (pos > 0 && message[pos - 1] == '\n') {
//             message[pos - 1] = '\0';
//             break;
//         }
//         pos > 0 ? pos-- : 0;
//         if (pos + 25 > size)
//             message = realloc(message, (size += size));
//         }
//         else
//             break;
//     }
//     return message;
// }

int mx_send_message_from_client(t_client_info *info, t_package *package, char *message) {
    int msg_size = strlen(message);

    if (msg_size > MX_MAX_DATA_SIZE) {
        printf("send more than 1 piece\n"); // #
        int pos_in_data = 0;

        package->piece = 1;
        strncat(package->data, message, MX_MAX_DATA_SIZE);
        write(info->socket, package, MX_PACKAGE_SIZE);
        while ((pos_in_data += MX_MAX_DATA_SIZE) < msg_size) {
            package->piece = pos_in_data + MX_MAX_DATA_SIZE < msg_size ? 2 : 3;
            mx_memset(package->data, 0, MX_MAX_DATA_SIZE);
            strncat(package->data, message + pos_in_data, MX_MAX_DATA_SIZE);
            write(info->socket, package, MX_PACKAGE_SIZE);
        }
    }
    else {
        printf("send 1 piece\n"); // #
        package->piece = 0;
        strncat(package->data, message, MX_MAX_DATA_SIZE);
        write(info->socket, package, MX_PACKAGE_SIZE);
    }
    return 0;
}

void mx_process_message_in_client(t_client_info *info) {
    char *message = NULL;
    t_package *package = mx_create_new_package();
    // tmp
    package->user_id = 1;
    strncat(package->login, info->login, sizeof(package->login) - 1);
    strncat(package->password, info->password, sizeof(package->password) - 1);
    //
    package->type = MX_MSG_TYPE;
    while(1) {
        message = strdup((char *)gtk_entry_get_text(GTK_ENTRY(info->data->message_entry)));
        mx_send_message_from_client(info, package, message);
        mx_strdel(&message);
        mx_memset(package->data, 0, sizeof(package->data));
        // tmp
        printf("back to menu\n");
        break;
        // mx_get_input(package->data);
        // printf("sizeof(package) = %zu\n", sizeof(package));
        // write(info->socket, package, MX_PACKAGE_SIZE);
        //
    }
    free(package);
}

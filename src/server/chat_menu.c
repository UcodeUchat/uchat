#include "inc/uchat.h"

void mx_chat_menu(t_info *info, t_clients *client_info) {
    char *input = NULL;

    client_info->channel = 0; // default channel (can write all clients)
    mx_strdel(&(client_info->password));
    while (1) {
        input = mx_read_client_input(client_info->fd);
        if (strcmp(input, "exit") == 0)
            write(client_info->fd, "You want exit!\n", 15);
        else
            mx_message_received(input, info, client_info);
        mx_strdel(&input);
    }
}

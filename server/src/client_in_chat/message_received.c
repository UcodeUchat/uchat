#include "../../inc/uchat.h"

static void send_message_to_all(char *msg, t_info *info, t_clients *client) {
    char *full_msg = malloc(strlen(msg) + strlen(client->login) + 5);

    sprintf(full_msg, "%s: %s\n", client->login, msg);
    pthread_mutex_lock(&info->mutex);
    for (t_clients *tmp = info->clients; tmp; tmp = tmp->next)
        if (client->channel == tmp->channel && client->tid != tmp->tid) // write to all channel members
            write(tmp->fd, full_msg, mx_strlen(full_msg));
    pthread_mutex_unlock(&info->mutex);
    mx_strdel(&full_msg);
}

void mx_message_received(char *message, t_info *info, t_clients *client_info) {
    char *ask = malloc(strlen(message) + strlen(client_info->login) + 55);

    send_message_to_all(message, info, client_info);
    sprintf(ask, "INSERT INTO msg_history (login, msg) values('%s', '%s')",
    client_info->login, message);
    if (sqlite3_exec(info->db, ask, NULL, NULL, NULL) != SQLITE_OK)
        fprintf(stderr, "Can't save message\n");
    mx_strdel(&ask);
}

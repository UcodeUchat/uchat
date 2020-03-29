#include "../inc/uchat.h"

static t_clients *find_client_in_list(t_info *info) {
    t_clients *client_list = NULL;
    pthread_t my_tid = pthread_self();

    pthread_mutex_lock(&info->mutex);
    for (t_clients *tmp = info->clients; tmp; tmp = tmp->next)
        if (tmp->tid == my_tid) {
            client_list = tmp;
            break;
        }
    pthread_mutex_unlock(&info->mutex);
    if (client_list == NULL) {
        fprintf(stderr, "Can't find client info in list\n");
        return NULL;
    }
    return client_list;
}

void *mx_start_client(void *argument) {
    t_info *info = (t_info *)argument;
    t_clients *client_info = find_client_in_list(info);
    char *input = malloc(1024);

    if (client_info) {
        mx_chat_connect(info, client_info);
        // read(client_info->fd, input, 1024);
        // if (strlen(input) < 2)
        //     break;
        // write(client_info->fd, input, strlen(input));
    }
    return NULL;
}

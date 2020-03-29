#include "inc/uchat.h"

t_clients *mx_user_online(t_info *info, char *login) {
    pthread_mutex_lock(&info->mutex);
    for (t_clients *tmp = info->clients; tmp; tmp = tmp->next) {
        if (tmp->login && strcmp(tmp->login, login) == 0 && tmp->channel != -1) {
            pthread_mutex_unlock(&info->mutex);
            return tmp;
        }
    }
    pthread_mutex_unlock(&info->mutex);
    return NULL;
}

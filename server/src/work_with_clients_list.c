#include "../inc/uchat.h"

t_clients *mx_create_new_client(int fd, pthread_t tid) {
    t_clients *new_client = (t_clients *)malloc(sizeof(t_clients));

    if (new_client) {
        new_client->fd = fd; // fd number
        new_client->tid = tid; // thread id
        new_client->channel = -1; // without channel
        new_client->login = NULL;
        new_client->password = NULL;
        new_client->err_msg = NULL;
        new_client->next = NULL;
    }
    return new_client;
}

void mx_push_client_back(t_clients **list, int fd, pthread_t tid) {
    t_clients *new_client = NULL;

    if (list && (new_client = mx_create_new_client(fd, tid)) != NULL) {
        if (*list) {
            t_clients *temp = *list;

            while (temp->next)
                temp = temp->next;
            temp->next = new_client;
        }
        else
            *list = new_client;
    }
}

void mx_pop_client_front(t_clients **head) {
    t_clients *temp = NULL;

    if (head && *head) {
        temp = *head;
        *head = temp->next;
        free(temp);
    }
}

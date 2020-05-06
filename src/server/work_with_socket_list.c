#include "uchat.h"

t_socket_list *mx_create_socket_elem(int socket, struct tls *tls_socket,
                                     t_socket_list *parent) {
    t_socket_list *new_elem = (t_socket_list *)malloc(sizeof(t_socket_list));

    new_elem->socket = socket;
    new_elem->tls_socket = tls_socket;
    new_elem->tok = json_tokener_new();
    new_elem->obj = NULL;
    new_elem->left = new_elem->right = NULL;
    new_elem->parent = parent;
    return new_elem;
}

void mx_add_socket_elem(t_socket_list **head, int sock, struct tls *tls_sock) {
    if (*head == NULL)
        *head = mx_create_socket_elem(sock, tls_sock, NULL);
    else {
        t_socket_list *tmp = *head;

        while (tmp) {
            if (sock > tmp->socket) {
                if (tmp->right)
                    tmp = tmp->right;
                else {
                    tmp->right = mx_create_socket_elem(sock, tls_sock, tmp);
                    return;
                }
            }
            else if (sock < tmp->socket) {
                if (tmp->left)
                    tmp = tmp->left;
                else {
                    tmp->left = mx_create_socket_elem(sock, tls_sock, tmp);
                    return;
                }
            }
            else {
                fprintf(stderr, "This socket exist in socket list:%d\n", sock);
                break;
            }
        }
    }
}

t_socket_list *mx_get_min_socket_elem(t_socket_list *head) {
    while (head->left) {
        head = head->left;
    }
    return head;
}

t_socket_list *mx_get_max_socket_elem(t_socket_list *head) {
    while (head->right) {
        head = head->right;
    }
    return head;
}

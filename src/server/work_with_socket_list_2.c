#include "uchat.h"

t_socket_list *mx_find_socket_elem(t_socket_list *head, int socket) {
    while (head) {
        if (head->socket > socket) {
            head = head->left;
            continue;
        }
        else if (head->socket < socket) {
            head = head->right;
            continue;
        }
        else {
            return head;
        }
    }
    return NULL;
}

t_socket_list* deleteNode(t_socket_list *head, int socket) {
    if (head == NULL)
        return head; 
    if (socket < head->socket)
        head->left = deleteNode(head->left, socket);
    else if (socket > head->socket)
        head->right = deleteNode(head->right, socket);
    else {
        if (head->left == NULL) {
            t_socket_list *temp = head->right;
            free(head);
            return temp;
        }
        else if (head->right == NULL) {
            t_socket_list *temp = head->left;
            free(head);
            return temp;
        }
        else {
            t_socket_list *temp = mx_get_min_socket_elem(head->right);

            head->socket = temp->socket;
            head->tls_socket = temp->tls_socket;
            head->right = deleteNode(head->right, temp->socket);
        }
    }
    return head;
}

void mx_delete_socket_elem(t_socket_list **head, int socket) {
    t_socket_list *elem_to_delete = mx_find_socket_elem(*head, socket);

    if (elem_to_delete && elem_to_delete->tls_socket) {
        close(socket);
        tls_close(elem_to_delete->tls_socket);
        tls_free(elem_to_delete->tls_socket);
    }
    *head = deleteNode(*head, socket);
}

void mx_print_socket_tree(t_socket_list *head, const char *dir, int level) {
    if (head) {
        printf("lvl %d %s = %d\n", level, dir, head->socket);
        mx_print_socket_tree(head->left, "left", level + 1);
        mx_print_socket_tree(head->right, "right", level + 1);
    }
}

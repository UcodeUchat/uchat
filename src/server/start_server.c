#include "uchat.h"

// add client_sock in struct kevent
static int add_new_client(t_server_info *info, struct tls *tls, int server) {
    struct tls *tls_socket;
    struct kevent new_ev;
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sock = accept(server, (struct sockaddr*) &client_address,
                             &client_len);

    if (client_sock == -1)
        mx_err_return2("error =", strerror(errno));
    printf("server new client socket %d\n", client_sock);
    mx_print_client_address(client_address, client_len);
    EV_SET(&new_ev, client_sock, EVFILT_READ, EV_ADD,0, 0, 0);
    if (kevent(info->kq, &new_ev, 1, 0, 0, NULL) == -1)
        mx_err_return2("error =", strerror(errno));
    tls_socket = NULL;
    if ((mx_make_tls_connect(tls, &tls_socket, client_sock)) != 0)
        return 1;
    mx_add_socket_elem(&(info->socket_list), client_sock, tls_socket);
    return 0;
}


static int work_with_client(t_server_info *info, struct kevent new_ev) {
    int rc;

    printf("\t\t\twork with client %d\n", (int) new_ev.ident);
    if ((new_ev.flags & EV_EOF) != 0) {
        printf("Client %lu disconnected\n", new_ev.ident);
        mx_drop_socket(info, new_ev.ident);
        mx_delete_socket_elem(&(info->socket_list), new_ev.ident);
    }
    else {
        rc = mx_tls_worker(mx_find_socket_elem(info->socket_list, new_ev.ident), info);
        if (rc == -1)
            mx_err_return2("error =", strerror(errno));
    }
    return 0;
}

static int config_kevent(t_server_info *info, int server) {
    struct kevent new_ev;

    if ((info->kq = kqueue()) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    // add server sock in struct kevent
    EV_SET(&new_ev, server, EVFILT_READ, EV_ADD, 0, 0, 0);
    if (kevent(info->kq, &new_ev, 1, 0, 0, NULL) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static void server_loop(t_server_info *info, int server, struct tls *tls) {
    struct timespec timeout;
    struct kevent new_ev;
    int event;

    timeout.tv_sec = 1;  // seconds
    timeout.tv_nsec = 0;  // nanoseconds
    while (1) {
        event = kevent(info->kq, NULL, 0, &new_ev, 1, &timeout);
        if (event == 0)  // check new event
            continue;
        if (event == -1) {
            printf("error = %s\n", strerror(errno));
            break;
        }
        if (new_ev.ident == (uintptr_t) server) {  // if new con - add new clt
            if ((add_new_client(info, tls, server)) != 0)
                break;
        }
        else {  // if read from client
            if ((work_with_client(info, new_ev)) != 0)
                break;
        }
    }
}

int mx_start_server(t_server_info *info) {
    int server_soc;
    struct tls *tls = NULL;
//    struct kevent new_ev;
//    int event;

    if ((mx_create_tls_configuration(&tls)) != 0)
        return -1;
    if ((server_soc = mx_create_server_socket(info)) == -1)
        return -1;
    if (listen(server_soc, SOMAXCONN) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }
    printf("listen fd = %d\n", server_soc);
    if ((config_kevent(info, server_soc) != 0)) {
        close(server_soc);
        return -1;
    }
    server_loop(info, server_soc, tls);
    /*
    struct timespec timeout;
    timeout.tv_sec = 1;  // seconds
    timeout.tv_nsec = 0;  // nanoseconds

    while (1) {
        event = kevent(info->kq, NULL, 0, &new_ev, 1, &timeout);
        if (event == 0)  // check new event
            continue;
        if (event == -1) {
            printf("error = %s\n", strerror(errno));
            break;
        }
        if (new_ev.ident == (uintptr_t) server) {  // if new connect - add new client
            if ((add_new_client(info, tls, server)) != 0)
                break;
        }
        else {  // if read from client
            if ((work_with_client(info, new_ev)) != 0)
                break;
        }
    }
     */
    close(info->kq);
    close(server_soc);
    return 0;
}

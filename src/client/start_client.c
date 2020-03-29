#include "inc/uchat.h"

static int connect_to_server(const struct sockaddr *addr, socklen_t alen) {
    int fd;
//    int err = 0;

    if ((fd = socket(addr->sa_family, SOCK_STREAM, IPPROTO_IP)) < 0)
        mx_err_exit("socket error\n");
    if (connect(fd, addr, alen) < 0)
        mx_err_exit("connect error\n");
    return fd;
}

void mx_start_client2(t_info_client *info) {
    char *client_input;

    struct sockaddr_in sa;
    inet_aton(info->ip, &(sa.sin_addr));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(info->port);
    info->fd = connect_to_server((struct sockaddr *)&sa, sizeof(sa));
    if (mx_server_input(info) == 0)
        while (1) {
            sleep(1);
            pthread_mutex_lock(&info->mutex);
            client_input = mx_get_client_input();
            pthread_mutex_unlock(&info->mutex);
            write(info->fd, client_input, strlen(client_input));
            mx_strdel(&client_input);
        }
    else
        fprintf(stderr, "Can't create thread");
}

#include "inc/uchat.h"

static void *get_server_input(void *argument) {
    t_info_client *info = (t_info_client *)argument;
    char *input = mx_strnew(1024);

    while (1) {
        read(info->fd, input, 1024);
        pthread_mutex_lock(&info->mutex);
        write(1, "Server:", 7);
        write(1, input, mx_strlen(input));
        pthread_mutex_unlock(&info->mutex);
        memset(input, 0, mx_strlen(input));
    }
}

int mx_server_input(t_info_client *info) {
    int thread_error;
    pthread_attr_t attr;
    pthread_t server_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if ((thread_error = pthread_create(&server_thread, &attr,
        get_server_input, info)) != 0) {
        printf("error pthread_create = %s\n", strerror(thread_error));
        close(info->fd);
        return 1;
    }
    return 0;
}

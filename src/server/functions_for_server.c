#include "uchat.h"

int mx_save_send(pthread_mutex_t *mutex, struct tls *tls_socket,
                 const char *content, int size) {
    int sended = 0;

    pthread_mutex_lock(mutex);
    if (size > 16384) {
        for (int i = 0; size > sended && i >= 0; sended += i) {
            i = tls_write(tls_socket, content + sended, size - sended);
        }
    }
    else
        sended = tls_write(tls_socket, content, size);
    pthread_mutex_unlock(mutex);
    return size > sended ? -1 : sended;
}

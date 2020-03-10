#include "uchat.h"

void * mx_worker(void *arg) {
    printf("New client created\n");
    int client_sock = (int) arg;
    while(1) {
        int buffer = 0;
        ssize_t size = read(client_sock, &buffer, sizeof(buffer));
        if (size == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        if (size == 0) {
            printf("Closed connection\n");
            break;
        }
        printf("Received %d\n", buffer);
        buffer++;
        if (write(client_sock, &buffer, sizeof(buffer)) == -1)
            printf("error = %s\n", strerror(errno));
        else
            printf("Sent %d\n", buffer);
    }
    close(client_sock);
    return NULL;
}

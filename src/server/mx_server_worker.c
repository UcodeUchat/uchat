#include "uchat.h"

int mx_tls_worker(struct tls *tls_accept) {
    char buf[1024];
    int rc;

    rc = tls_read(tls_accept, buf, sizeof(buf));	// get request
    if (rc > 0 ) {
        buf[rc] = 0;
        printf("Client msg: %s\n", buf);
        tls_write(tls_accept, buf, strlen(buf));    // send reply
    }
    if (rc == -1 ) {
        tls_close(tls_accept);
        tls_free(tls_accept);
    }
    return 0;
}

void *mx_worker(void *arg) {
    int client_sock = *((int *)arg);
    ssize_t size;
    time_t now;

    printf("run worker\n");
    printf("client_sock = %d\n", client_sock);
    printf("New client created\n");

    pthread_t thread_id = pthread_self();
    pid_t pid = getpid();
    printf("pid %d, tid %d: new thread, client socket = %d\n",
           (int)pid, (int)thread_id, *((int *) arg));

    while(1) {
        int buffer = 0;
        if ((size = read(client_sock, &buffer, sizeof(buffer))) == -1) {
            printf("error1 = %s\n", strerror(errno));
            continue;
        }
        if (size == 0) {
            printf("Closed connection\n");
            break;
        }
        now = time(0);
        char * time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0';
        printf("%s\tReceived %d\n", time_str, buffer);
        buffer++;
        if (write(client_sock, &buffer, sizeof(buffer)) == -1)
            printf("error2 = %s\n", strerror(errno));
        else
            printf("Sent %d\n\n", buffer);
    }
    close(client_sock);
    return NULL;
}

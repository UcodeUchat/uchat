#include "uchat.h"

void *mx_worker(void *arg) {
    int client_sock = *((int *)arg);
//    ssize_t size;
    time_t now;


    printf("New client created\n");
    printf("client_sock = %d\n", client_sock);

    pthread_t thread_id = pthread_self();
    pid_t pid = getpid();
    printf("pid %d, tid %d: new thread, client socket = %d\n",
           (int)pid, (int)thread_id, *((int *) arg));

    while(1) {
//        int buffer = 0;
        char read[1024];
        int bytes_received = recv(client_sock, read, 1024, 0);
        if (bytes_received < 1) {
            break;
        }

        /*
        if ((size = read(client_sock, &buffer, sizeof(buffer))) == -1) {
            printf("error1 = %s\n", strerror(errno));
            continue;
        }
        if (size == 0) {
            printf("Closed connection\n");
            break;
        }
         */
        now = time(0);
        char * time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0';
        printf("%s\tReceived %s\n", time_str, read);

        int bytes_sent = send(client_sock, read, strlen(read), 0);
        printf("Sent %d bytes.\n", bytes_sent);

        /*
        if (write(client_sock, &buffer, sizeof(buffer)) == -1)
            printf("error2 = %s\n", strerror(errno));
        else
            printf("Sent %d\n\n", buffer);
        */
    }
    close(client_sock);
    return NULL;
}


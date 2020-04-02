#include "uchat.h"

int mx_worker(int client_sock) {
    ssize_t size;
    int buffer;

    printf("client_sock = %d\n", client_sock);
    now = time(0);
    char * time_str = ctime(&now);
    time_str[strlen(time_str)-1] = '\0';

    size = read(client_sock, &buffer, sizeof(buffer));
    if (size == -1)
        return -1;
    printf("%s\tReceived %s\n", time_str, buffer);
    int bytes_sent = send(client_sock, buffer, strlen(buffer), 0);

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


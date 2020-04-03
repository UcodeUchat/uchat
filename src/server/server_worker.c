#include "uchat.h"

int mx_worker(int client_sock) {
    ssize_t size;
    char client_input[MAX_CLIENT_INPUT];
    time_t now;

    now = time(0);
    char * time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';

    size = read(client_sock, &client_input, MAX_CLIENT_INPUT);
    if (size == -1)
        return -1;
    printf("%s\tReceived %s\n", time_str, client_input);

    size = write(client_sock, client_input, sizeof(client_input));
    return (int) size;
}


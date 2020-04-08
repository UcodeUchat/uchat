#include "uchat.h"

int mx_worker(int client_sock) {
    ssize_t size = 0;
    char client_input[MAX_CLIENT_INPUT];
    
    size = read(client_sock, &client_input, MAX_CLIENT_INPUT);
    if (size == -1)
        return -1;
    if (size == 0)
        return 0;
    client_input[size] = '\0';
    printf("Received %s\n", client_input);
    if ((mx_check_client(client_sock, client_input)) == 1) {
        printf("client loggin\n");
        return 0;
    }
    mx_print_curr_time();
    size = write(client_sock, client_input, sizeof(client_input));
    return (int) size;
}


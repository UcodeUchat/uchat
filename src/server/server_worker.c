#include "uchat.h"

int mx_worker(int client_sock, t_server_info *info) {
    ssize_t size = 0;
    char client_input[MAX_CLIENT_INPUT];
    
    size = read(client_sock, &client_input, MAX_CLIENT_INPUT);
    client_input[size] = '\0';
    if (size == -1)
        return -1;
    if ((mx_check_client(client_sock, client_input, info)) == 1) {
        printf("client login\n");
//        return 0;
//    }
        mx_print_curr_time();
        printf("Received %s\n", client_input);

        size = write(client_sock, client_input, sizeof(client_input));
        return (int) size;
    }
    else
        return 0;
}


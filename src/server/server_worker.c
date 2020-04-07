#include "uchat.h"

int mx_worker(int client_sock) {
    ssize_t size = 0;
    char client_input[MAX_CLIENT_INPUT];
    int autorized = 0;

    //if check_user in db  == -1 -
    if ((autorized = mx_check_client(client_sock)) == -1)
        printf("client not loggin\n");
//        return 0;
//    autorized = 1;
    if(autorized != 0) {
        size = read(client_sock, &client_input, MAX_CLIENT_INPUT);
        if (size == -1)
            return -1;
        mx_print_curr_time();
        printf("Received %s\n", client_input);

        size = write(client_sock, client_input, sizeof(client_input));
        return (int) size;
    }
    else
        return 0;
}


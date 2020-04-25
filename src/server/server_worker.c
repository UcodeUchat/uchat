#include "uchat.h"

int mx_tls_worker(t_socket_list *client_socket_list, t_server_info *info) {
    t_package *new_package = malloc(MX_PACKAGE_SIZE);
    int rc;

    rc = tls_read(client_socket_list->tls_socket, new_package, MX_PACKAGE_SIZE);    // get request
    if (rc == -1) {
        tls_close(client_socket_list->tls_socket);
        tls_free(client_socket_list->tls_socket);
    }
    if (rc > 0) {
        new_package->client_sock = client_socket_list->socket;
        new_package->client_tls_sock = client_socket_list->tls_socket;
        mx_run_function_type(info, new_package);
    }
    else
        printf("Readed 0 bytes\n");
    free(new_package);
    return 0;
}


int mx_worker(int client_sock, t_server_info *info) {
    t_package *new_package = malloc(MX_PACKAGE_SIZE);

    int rc = recv(client_sock, new_package, MX_PACKAGE_SIZE, MSG_WAITALL);
    if (rc == -1)
        mx_err_exit("error recv\n");

    new_package->client_sock = client_sock; // #
    // check_is_it_correct_data
    // mx_memset(new_package->password, 0, sizeof(new_package->password));
    //

    fprintf(stderr, "LOGIN!!!\n");
    fprintf(stderr, "log = [%s]\n", new_package->login);
    fprintf(stderr, "pas = [%s]\n", new_package->password);
    mx_run_function_type(info, new_package);
    free(new_package);
    return 0;
//     ssize_t size = 0;
//     char client_input[MAX_CLIENT_INPUT];
    
//     size = read(client_sock, &client_input, MAX_CLIENT_INPUT);
//     client_input[size] = '\0';
//     if (size == -1)
//         return -1;
//     if ((mx_check_client(client_sock, client_input, info)) == 1) {
//         printf("client login\n");
// //        return 0;
// //    }
//         mx_print_curr_time();
//         printf("Received %s\n", client_input);

//         size = write(client_sock, client_input, sizeof(client_input));
//         return (int) size;
//     }
//     else
//         return 0;
}

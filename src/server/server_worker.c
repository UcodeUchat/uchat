#include "uchat.h"

/*
 * typedef struct  s_package {
    struct tls *client_tls_sock; // #
    int client_sock; // #
    char type; // input type
    char piece; // 0 - full, 1 - start, 2 - partition, 3 - end
    int user_id; // sender unical id
    int room_id; // room unical id
    int add_info; // addition information which use different package types
    char login[50]; // user login
    char password[32]; // user password
    char data[1024]; // user data
    struct  s_server_room *rooms;
}               t_package;
 */
int mx_tls_worker(t_socket_list *client_socket_list, t_server_info *info) {
    t_package *new_package = malloc(MX_PACKAGE_SIZE);
    int rc;
    char buf[10124];
    json_object *new_obj;
//    char json_str;

    rc = tls_read(client_socket_list->tls_socket, new_package, MX_PACKAGE_SIZE);    // get request
//    rc = tls_read(client_socket_list->tls_socket, buf, 1024);    // get request

    new_obj = json_tokener_parse(buf);
    mx_print_json_object(new_obj, "json from client");

    if (rc == -1) {
        tls_close(client_socket_list->tls_socket);
        tls_free(client_socket_list->tls_socket);
    }
    if (rc > 0) {

        new_package->client_sock = client_socket_list->socket;
        new_package->client_tls_sock = client_socket_list->tls_socket;
        new_package->type = json_object_get_int(json_object_object_get(new_obj, "type"));
        new_package->login = strdup(json_object_to_json_string(json_object_object_get(new_obj, "login")));
        new_package->password = strdup(json_object_to_json_string(json_object_object_get(new_obj, "password")));
        new_package->data = strdup(json_object_to_json_string(json_object_object_get(new_obj, "data")));

        mx_run_function_type(info, new_package);
    }
    else
        printf("Readed 0 bytes\n");
    free(new_package);
    return 0;
}

/*
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
*/

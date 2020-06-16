#include "uchat.h"

static int connect_client_loop(struct addrinfo *peer_address) {
    int numsec;
    int sock;
    int enable = 1;

    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        if ((sock = socket(peer_address->ai_family, peer_address->ai_socktype,
                           peer_address->ai_protocol)) < 0) {
            freeaddrinfo(peer_address);
            return -1;
        }
        setsockopt(sock, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(int));
        if ((connect(sock, peer_address->ai_addr,
                     peer_address->ai_addrlen)) == 0)
            return sock;
        printf("not connect\n");
        close(sock);
        if (numsec <= MAXSLEEP / 2)
            sleep(numsec);
    }
    return -1;
}


int mx_connect_client(t_client_info *info) {
    struct addrinfo hints;
    struct addrinfo *peer_address = NULL;
    int sock;
    int err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(info->ip, info->argv[2],
                           &hints, &peer_address)) != 0) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", gai_strerror(err));
        return -1;
    }
    if ((sock = connect_client_loop(peer_address)) == -1)
        return -1;
    printf("connect to server cocket %d\n", sock);
    return sock;
}

static void clean_client(t_client_info *info) {
    tls_close(info->tls_client);
    tls_free(info->tls_client);
    close(info->socket);
}



static int mx_start_reg(t_client_info *info) {
//    info->login = strdup(info->argv[3]);
    info->password = strdup(mx_strhash("000000"));
    info->current_room = 0;
    printf("mx_start_reg\n");

    json_object *json_obj = mx_create_basic_json_object(MX_REG_TYPE);
    const char *json_str;
    mx_js_o_o_add(json_obj, "login", mx_js_n_str(info->login));
    mx_js_o_o_add(json_obj, "password", mx_js_n_str( info->password));
    json_str = mx_js_o_to_js_str(json_obj);
    tls_send(info->tls_client, json_str, strlen(json_str));
    return 0;
}


static int mx_start_authorization(t_client_info *info) {
    printf("mx_start_avtorization\n");
    json_object *json_obj = mx_create_basic_json_object(MX_AUTH_TYPE);
    const char *json_str;

    mx_js_o_o_add(json_obj, "login", mx_js_n_str(info->login));
    mx_js_o_o_add(json_obj, "password", mx_js_n_str(info->password));
    json_str = mx_js_o_to_js_str(json_obj);
    tls_send(info->tls_client, json_str, strlen(json_str));
    return 0;
}


void mx_send_message_from_client(t_client_info *info) {
//    char *message = NULL;
    const char *json_string;
    json_object *obj = mx_create_basic_json_object(MX_MSG_TYPE);

    printf("mx_send_message_from_client\n");
    mx_js_o_o_add(obj, "user_id", mx_js_n_int(info->id));
    mx_js_o_o_add(obj, "room_id", mx_js_n_int(info->current_room));
    mx_js_o_o_add(obj, "login", mx_js_n_str(info->login));
    mx_js_o_o_add(obj, "add_info", mx_js_n_int(0));
    char message[100];

    snprintf(message, 100, "%d", rand());
    mx_js_o_o_add(obj, "data", mx_js_n_str(message));
    json_string = mx_js_o_to_js_str(obj);
    tls_send(info->tls_client, json_string, strlen(json_string));
//    mx_strdel(&message);
    json_object_put(obj);
}

static void mx_start_tests(t_client_info *info) {
//    int a = 0;
    int res;
    res = mx_start_reg(info);
    printf ("\r\x1B[32m test registration: %s\n\x1B[0m", res == 0 ? "OK" : "FAILED");
    res = 0;
    res = mx_start_authorization(info);

    printf ("\r\x1B[32m test avtorization: %s \n\x1B[0m", res == 0 ? "OK" : "FAILED");
    sleep(250);
    /*
    sleep(2);
    while (a < 5) {
        a++;
        sleep(1);
        mx_send_message_from_client(info);
    }
    printf("test send message OK\n");
    mx_send_file_from_bot(info, "uchat");
    printf("test send file OK\n");
     */
}

int mx_start_client(t_client_info *info) {
    pthread_t thread_input;
    pthread_attr_t attr;
    int tc;

    if (mx_tls_config_client(info))  // conf tls
        return 1;
    info->socket = mx_connect_client(info);
    if(info->socket == -1)
        return 1;
    if (mx_make_tls_connect_client(info)) // tls connect and handshake
        return 1;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // #
    tc = pthread_create(&thread_input, &attr, mx_process_input_from_server, info);
    if (tc != 0)
        printf("pthread_create error = %s\n", strerror(tc));
    mx_start_tests(info);
    //    mx_login(info);
    pthread_cancel(thread_input);
    clean_client(info);
    return 0;
}




#include "uchat.h"

int mx_detect_file_extention(char *filename) {
    char *dot = strrchr(filename, '.');

    if (!dot || dot == filename)
        return -1;
    else {
        while (mx_get_char_index(dot + 1, '.') >= 0) {
            char *tmp = strdup(dot + mx_get_char_index(dot, '.'));
            free(dot);
            dot = strdup(tmp);
            free(tmp); 
        }
        dot++;
        if (!strcmp(dot, "jpg") || !strcmp(dot, "jpeg") ||\
            !strcmp(dot, "png") || !strcmp(dot, "gif"))
            return 1;
        else if (!strcmp(dot, "aif"))
            return 2;
        else
            return 0;
    }
}

int tls_send(struct tls *tls_socket, const char *content, int size) {
    int sended = 0;

    if (size > 16384) {
        for (int i = 0; size > sended && i >= 0; sended += i) {
            i = tls_write(tls_socket, content + sended, size - sended);
        }
    }
    else
        sended = tls_write(tls_socket, content, size);
    return size > sended ? -1 : sended;
}

void mx_print_client_address(struct sockaddr_storage client_address,
                             socklen_t client_len) {
    char host[100];
    char serv[100];

    getnameinfo((struct sockaddr*)&client_address, client_len,
                host, sizeof(host), serv, sizeof(serv),
                NI_NUMERICHOST | NI_NUMERICSERV);
    printf("New connection from %s %s\n", host, serv);
}


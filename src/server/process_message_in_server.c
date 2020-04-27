#include "uchat.h"

int mx_process_message_in_server(t_server_info *info, t_package *package) {
    (void)info;
    printf("msg from %s, piece=%d\n", package->login, package->piece);
    printf("data : %s\n", package->data);

//    char *mail_data = "You have a message in UCHAT.\r\n";
    pthread_t thread_input;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int tc = pthread_create(&thread_input, &attr, mx_send_mail, strdup(package->data));
    if (tc != 0)
        printf("error = %s\n", strerror(tc));
    mx_print_tid("thread for send mail");

    tls_write(package->client_tls_sock, package, MX_PACKAGE_SIZE);
    return 0;
}

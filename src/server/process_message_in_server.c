#include "uchat.h"

int mx_process_message_in_server(t_server_info *info, t_package *package) {
    (void)info;
    printf("msg from %s, piece=%d\n", package->login, package->piece);
    printf("data : %s\n", package->data);

//    char *mail_data = "You have a message in UCHAT.\r\n";
    mx_send_mail(package->data);
    tls_write(package->client_tls_sock, package, MX_PACKAGE_SIZE);
    return 0;
}

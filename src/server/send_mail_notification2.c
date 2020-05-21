#include "uchat.h"

int mx_err_return(const char *err_msg) {
    mx_printerr(err_msg);
    return 1;
}

void mx_send_format(int socket, const char *text) {
    send(socket, text, strlen(text), 0);
    printf("C: %s", text);
}

void mx_init_struct_mail(t_mail *mail, char *receiver, char *message) {
    memset(mail, 0, sizeof(t_mail));
    mail->hostname = strdup("smtp.gmail.com");
    mail->sender = strdup("uchat.server@gmail.com");
    mail->receiver = strdup(receiver);
    mail->subject = strdup("UCHAT");
    mail->message = strdup(message);
}

void mx_send_format_tls(struct tls *tls, const char *text, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

//    strcpy(buffer, text);
//    strcpy(buffer + )
    tls_write(tls, buffer, strlen(buffer));
    printf("C: %s", buffer);
}

int mx_check_response(const char *response) {
    const char *k = response;

    if (!k[0] || !k[1] || !k[2]) return 0;
    for (; k[3]; ++k) {
        if (k == response || k[-1] == '\n') {
            if (isdigit(k[0]) && isdigit(k[1]) && isdigit(k[2])) {
                if (k[3] != '-') {
                    if (strstr(k, "\r\n")) {
                        return strtol(k, 0, 10);
                    }
                }
            }
        }
    }
    return 0;
}


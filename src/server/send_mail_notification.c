#include "uchat.h"

static int send_mail_connect(t_mail * mail) {
    int server = 0;

    printf("Connecting to host: %s: %s\n", mail->hostname, "587");
    if ((server = mx_connect_to_server(mail->hostname, "587")) == 1)
        mx_err_return("failed connect ot hostname\n");
    if ((mx_wait_on_response(server, NULL, 220)) != 0)
        return 1;
    mx_send_format(server, "EHLO client.example.com\r\n");
    if ((mx_wait_on_response(server, NULL, 250)) != 0)
        return 1;
    mx_send_format(server, "STARTTLS\r\n");
    if ((mx_wait_on_response(server, NULL, 220)) != 0)
        return 1;
    return server;
}

static int send_mail_autentification(struct tls *tls_c) {
    mx_send_format_tls(tls_c, "EHLO client.example.com\r\n");
    if ((mx_wait_on_response(0, tls_c, 250)) != 0)
        return 1;
    mx_send_format_tls(tls_c, "AUTH LOGIN\r\n");
    if ((mx_wait_on_response(0, tls_c, 334)) != 0)
        return 1;
    mx_send_format_tls(tls_c, "dWNoYXQuc2VydmVyQGdtYWlsLmNvbQ==\r\n");
    if ((mx_wait_on_response(0, tls_c, 334)) != 0)
        return 1;
    mx_send_format_tls(tls_c, "MTk5NDI0MjRx\r\n");
    if ((mx_wait_on_response(0, tls_c, 235)) != 0)
        return 1;
    return 0;
}

static int send_mail_massage(struct tls *tls_c, t_mail *mail) {
    mx_send_format_tls(tls_c, "MAIL FROM:<%s>\r\n", mail->sender);
    if ((mx_wait_on_response(0, tls_c, 250)) != 0)
        return 1;
    mx_send_format_tls(tls_c, "RCPT TO:<%s>\r\n", mail->receiver);
    if ((mx_wait_on_response(0, tls_c, 250)) != 0)
        return 1;
    mx_send_format_tls(tls_c, "DATA\r\n");
    if ((mx_wait_on_response(0, tls_c, 354)) != 0)
        return 1;
    mx_send_format_tls(tls_c, "From:<%s>\r\n", mail->sender);
    mx_send_format_tls(tls_c, "To:<%s>\r\n", mail->receiver);
    mx_send_format_tls(tls_c, "Subject:%s\r\n", mail->subject);
    mx_send_format_tls(tls_c, "Mime-Version:%s\r\n", "1.0");
    mx_send_format_tls(tls_c, "Content-Type:%s\r\n", "text/html; charset=\"ISO-8859-1\"");
    mx_send_format_tls(tls_c, "Content-Transfer-Encoding:%s\r\n", "7bit");
    mx_send_format_tls(tls_c, "Date:%s\r\n", mx_date_to_char());
    if (mx_mail_data_sending(tls_c, mail) != MX_OK)
        return 1;
    mx_send_format_tls(tls_c, ".\r\n");
    return 0;
}

static int send_mail_quit(struct tls *tls_c) {
    if ((mx_wait_on_response(0, tls_c, 250)) != 0)
        return 1;
    mx_send_format_tls(tls_c, "QUIT\r\n", NULL);
    if ((mx_wait_on_response(0, tls_c, 221)) != 0)
        return 1;
    printf("\nClosing socket...\n");
    printf("Finish send mail.\n");
    return 0;
}

void *mx_send_mail(char *login, char *receiver, char *message) {
    int server;
    struct tls *tls_c = NULL;
    t_mail *mail = NULL;

    mail = (t_mail *)malloc(sizeof(t_mail));
    mx_init_struct_mail(mail, receiver, message);
    mail->user = login ? strdup(login) : NULL;
    if ((server = send_mail_connect(mail)) == 1)
        return NULL;
    tls_c = mx_create_tls();
    if (tls_connect_socket(tls_c, server, mail->sender) < 0) {
        printf("%s\n", tls_error(tls_c));
        return NULL;
    }
    if (send_mail_autentification(tls_c))
       return NULL;
    if (send_mail_massage(tls_c, mail)) // send massage
        return NULL;
    if (send_mail_quit(tls_c)) // send quit
        return NULL;
    close(server);
    tls_free(tls_c);
    return NULL;
}

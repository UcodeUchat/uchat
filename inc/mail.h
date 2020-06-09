#ifndef UCHAT_MAIL_H
#define UCHAT_MAIL_H

#define MAXRESPONSE 1024

typedef struct s_mail {
    char *hostname;
    char *sender;
    char *receiver;
    char *subject;
    char *message;
    char *user;
}           t_mail;


void *mx_send_mail(char *login, char *receiver, char *message);
int mx_mail_data_sending(struct tls *tls_c, t_mail *mail);
void mx_send_format(int socket, const char *text);
void mx_send_format_tls(struct tls *tls, char *arg1, char *arg2,
                        char *arg3);
void mx_init_struct_mail(t_mail *mail, char *receiver, char *message);
int mx_connect_to_server(const char *hostname, const char *port);
struct tls *mx_create_tls(void);
int mx_check_response(const char *response);
int mx_wait_on_response(int socket, struct tls *tls, int reply_code);

#endif

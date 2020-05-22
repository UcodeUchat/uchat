#ifndef UCHAT_MAIL_H
#define UCHAT_MAIL_H

#define MAXRESPONSE 1024

typedef struct s_mail {
    char *hostname;
    char *sender;
    char *receiver;
    char *subject;
    char *message;
}           t_mail;


void *mx_send_mail(char *receiver, char *message);
void mx_send_format(int socket, const char *text);
void mx_send_format_tls(struct tls *tls, const char *text, ...);
void mx_init_struct_mail(t_mail *mail, char *receiver, char *message);
int mx_connect_to_server(const char *hostname, const char *port);
struct tls *mx_create_tls(void);
int mx_check_response(const char *response);
int mx_wait_on_response(int socket, struct tls *tls, int reply_code);

#endif

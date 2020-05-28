#include "uchat.h"

int open_file_and_get_size(FILE **file, int *size, t_mail *mail, char **data) {
    if ((*file = fopen("./emails/universal.txt", "r")) != NULL) {
        fseek(*file, 0L, SEEK_END);
        *size = ftell(*file);
        fseek(*file, 0, SEEK_SET);
        *data = malloc((int)(*size + strlen(mail->message)
                       + strlen(mail->user) + 1));
        (*data)[*size + strlen(mail->message) + strlen(mail->user)] = '\0';
        return 0;
    }
    return 1;
}

char *get_html_data(t_mail *mail, int *sz) {
    FILE *email_file = NULL;
    char *data = NULL;
    int rd = 0;
    int size = 0;
   
    if (open_file_and_get_size(&email_file, &size, mail, &data) == MX_OK) {
        *sz = size + strlen(mail->message) + strlen(mail->user);
        if ((rd = fread(data , 1, 8126, email_file)) == 8126) {
            strcat(data + 8126, mail->user);
            if ((rd = fread(data + 8126 + strlen(mail->user), 1, 396,
                email_file)) == 396) {
                strcat(data + 8126 + 396 + strlen(mail->user), mail->message);
                size -= 8126 + 396;
                if ((rd = fread(data + 8126 + 396 + strlen(mail->user)
                    + strlen(mail->message), 1, size, email_file)) == size)
                    return data;
            }
        }
    }
    return NULL;
}

int mx_mail_data_sending(struct tls *tls_c, t_mail *mail) {
    int full_data_size = 0;
    char *html_data;

    if (!(mail->user))
        mail->user = strdup("User");
    if ((html_data = get_html_data(mail, &full_data_size))) {
        // mx_send_format_tls(tls_c, "%s\r\n", html_data);
        tls_write(tls_c, html_data, full_data_size);
        tls_write(tls_c, "\r\n", 2);
        mx_strdel(&html_data);
        return 0;
    }
    return -1;
}

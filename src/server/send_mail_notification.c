#include "uchat.h"

int mx_err_return(const char *err_msg) {
    mx_printerr(err_msg);
    return 1;
}

static void send_format(int socket, const char *text) {
    send(socket, text, strlen(text), 0);
    printf("C: %s", text);
}

static void send_format_tls(struct tls *tls, const char *text, ...) {
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


static int check_response(const char *response) {
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

static int connect_to_server(const char *hostname, const char *port) {
    struct addrinfo *peer_address;
    struct addrinfo hints;
    int server;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(hostname, port, &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", strerror(errno));
        return 1;
    }
    server = socket(peer_address->ai_family,
                    peer_address->ai_socktype, peer_address->ai_protocol);
    if (server == -1)
        mx_err_return(strerror(errno));
    if (connect(server, peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%s)\n", strerror(errno));
        return 1;
    }
    freeaddrinfo(peer_address);
    return server;
}

static int wait_on_response_tls(struct tls *tls, int reply_code) {
    char response[MAXRESPONSE + 1];
    char *p = response;
    char *end = response + MAXRESPONSE;
    int code = 0;
    int bytes_received;

    while (code == 0) {
        bytes_received = tls_read(tls, p, end - p);
        if (bytes_received < 1)
            mx_err_return("Connection dropped.\n");
        p += bytes_received;
        *p = 0;
        if (p == end) {
            fprintf(stderr, "Server response too large:\n");
            fprintf(stderr, "%s", response);
            return 1;
        }
        code = check_response(response);
    }
    if (code != reply_code) {
        fprintf(stderr, "Error from server:\n");
        fprintf(stderr, "%s", response);
        return 1;
    }
    printf("S: %s", response);
    return 0;
}

static int wait_on_response(int socket, int reply_code) {
    char response[MAXRESPONSE + 1];
    char *p = response;
    char *end = response + MAXRESPONSE;
    int code = 0;
    int bytes_received;

    while (code == 0) {
        bytes_received = recv(socket, p, end - p, 0);
        if (bytes_received < 1)
            mx_err_return("Connection dropped.\n");
        p += bytes_received;
        *p = 0;
        if (p == end) {
            fprintf(stderr, "Server response too large:\n");
            fprintf(stderr, "%s", response);
            return 1;
        }
        code = check_response(response);
    }
    if (code != reply_code) {
        fprintf(stderr, "Error from server:\n");
        fprintf(stderr, "%s", response);
        return 1;
    }
    printf("S: %s", response);
    return 0;
}

static struct tls *create_tls(void) {
    struct tls *tls_c = NULL;
    struct tls_config *config = NULL;

    if ((config = tls_config_new()) == NULL)
        mx_err_exit("unable to allocate config");
    if ((tls_c = tls_client()) == NULL)
        mx_err_exit("tls client creation failed");
    tls_config_insecure_noverifycert(config);
    tls_config_insecure_noverifyname(config);
    if (tls_configure(tls_c, config) != 0) {
        tls_config_free(config);
        printf("tls_configure error: %s\n", tls_error(tls_c));
        exit(1);
    }
    tls_config_free(config);
    return tls_c;
}

void *mx_send_mail(char *receiver, void *mess) {
    char *message = (char *)mess;
    int server;
    struct tls *tls_c = NULL;
    char *hostname = "smtp.gmail.com";
    char *sender = "uchat.server@gmail.com";
//    char *receiver = "zempro911@gmail.com";
    char *subject = "UCHAT";

    printf("Connecting to host: %s: %s5\n", hostname, "587");
    if ((server = connect_to_server(hostname, "587")) == 1)
        mx_err_return("failed to send mail\n");
    if ((wait_on_response(server, 220)) != 0)
        return NULL;
    send_format(server, "EHLO client.example.com\r\n");
    if ((wait_on_response(server, 250)) != 0)
        return NULL;
    send_format(server, "STARTTLS\r\n");
    if ((wait_on_response(server, 220)) != 0)
        return NULL;
    tls_c = create_tls();
    if (tls_connect_socket(tls_c, server, sender) < 0) {
        printf("tls_connect error\n");
        printf("%s\n", tls_error(tls_c));
        return NULL;
    }
    mx_report_tls(tls_c, hostname);
    send_format_tls(tls_c, "EHLO client.example.com\r\n");
    if ((wait_on_response_tls(tls_c, 250)) != 0)
        return NULL;

    send_format_tls(tls_c, "AUTH LOGIN\r\n");
    if ((wait_on_response_tls(tls_c, 334)) != 0)
        return NULL;

    send_format_tls(tls_c, "dWNoYXQuc2VydmVyQGdtYWlsLmNvbQ==\r\n");
    if ((wait_on_response_tls(tls_c, 334)) != 0)
        return NULL;

    send_format_tls(tls_c, "MTk5NDI0MjRx\r\n");
    if ((wait_on_response_tls(tls_c, 235)) != 0)
        return NULL;

    send_format_tls(tls_c, "MAIL FROM:<%s>\r\n", sender);
    if ((wait_on_response_tls(tls_c, 250)) != 0)
    return NULL;

    send_format_tls(tls_c, "RCPT TO:<%s>\r\n", receiver);
    if ((wait_on_response_tls(tls_c, 250)) != 0)
        return NULL;

    send_format_tls(tls_c, "DATA\r\n");
    if ((wait_on_response_tls(tls_c, 354)) != 0)
        return NULL;

    send_format_tls(tls_c, "From:<%s>\r\n", sender);
    send_format_tls(tls_c, "To:<%s>\r\n", receiver);
    send_format_tls(tls_c, "Subject:%s\r\n", subject);
    send_format_tls(tls_c, "\r\n");
    send_format_tls(tls_c, "Date:%s\r\n", mx_date_to_char());
    send_format_tls(tls_c, "%s\r\n", message);
    send_format_tls(tls_c, ".\r\n");
    if ((wait_on_response_tls(tls_c, 250)) != 0)
        return NULL;

    send_format_tls(tls_c, "QUIT\r\n", NULL);
    if ((wait_on_response_tls(tls_c, 221)) != 0)
        return NULL;

    printf("\nClosing socket...\n");
    close(server);
    tls_free(tls_c);
    printf("Finish send mail.\n");
    return NULL;
}


/*
 *
    // char *hostname = "alt1.gmail-smtp-in.l.google.com.";

    emVtcHJvOTEyQGdtYWlsLmNvbQ==
    NDE0OTUwMDA=
    run in shell
    perl -MMIME::Base64 -e 'print encode_base64("uchat.server@gmail.com");'
    perl -MMIME::Base64 -e 'print encode_base64("password");'

 */

/*
static void send_format_tls(struct tls *tls, char *text1, char *text2) {
    char buffer[1024];
    ssize_t  len = strlen(text1);
//    va_list args;
//    va_start(args, text);
//    vsprintf(buffer, text, args);
//    va_end(args);

    strcpy(buffer, text1);
    if (text2) {
        strcpy(buffer + len, text2);
        len += strlen(text2);
    }
    printf("\tbuffer!!!!! =%s\n", buffer);
    if (buffer[len ] != '\n') {
        strcpy(buffer + len, "\r\n");
        len += 2;
    }
    tls_write(tls, buffer, len);
    printf("C: %s", buffer);
}
 */

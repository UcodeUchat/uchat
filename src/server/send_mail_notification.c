#include "uchat.h"

/*
static void smtp_tls_config(struct tls tls_client) {
    struct tls_config *config = NULL;

    if ((config = tls_config_new()) == NULL)
        mx_err_exit("unable to allocate config");
    if ((tls_client = tls_client()) == NULL)
        mx_err_exit("tls client creation failed");
//    tls_config_insecure_noverifycert(config);
//    tls_config_insecure_noverifyname(config);
//    if (tls_config_set_dheparams(config, "auto") != 0)
//        mx_err_exit(tls_config_error(config));
//    if (tls_config_set_key_file(config, "./CA/client.key") != 0)
//        mx_err_exit(tls_config_error(config));
//    if (tls_config_set_cert_file(config, "./CA/client.pem") != 0)
//        mx_err_exit(tls_config_error(config));
    if (tls_configure(tls_client, config) != 0) {
        printf("tls_configure error: %s\n", tls_error(tls_client));
        exit(1);
    }
    tls_config_free(config);
}

 */

static void send_format(int socket, const char *text, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    send(socket, buffer, strlen(buffer), 0);

    printf("C: %s", buffer);
}

static void send_format_tls(struct tls *tls, const char *text, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

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
    struct addrinfo hints;
    int server;

    printf("Configuring remote address...\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(hostname, port, &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%s)\n", strerror(errno));
        return -1;
    }

    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);

    printf("Creating socket\n");
    server = socket(peer_address->ai_family,
                    peer_address->ai_socktype, peer_address->ai_protocol);
    if (server == -1) {
        printf("socket error = %s\n", strerror(errno));
        return -1;
    }

    if (connect(server,
                peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%s)\n", strerror(errno));
        return -1;
    }
    freeaddrinfo(peer_address);

    printf("Connected.\n\n");
    return server;
}

static void wait_on_response_tls(struct tls *tls, int reply_code) {
    char response[MAXRESPONSE + 1];
    char *p = response;
    char *end = response + MAXRESPONSE;
    int code = 0;

    while (code == 0) {
        int bytes_received = tls_read(tls, p, end - p);
        if (bytes_received < 1) {
            fprintf(stderr, "Connection dropped.\n");
            exit(1);
        }
        p += bytes_received;
        *p = 0;
        if (p == end) {
            fprintf(stderr, "Server response too large:\n");
            fprintf(stderr, "%s", response);
            exit(1);
        }
        code = check_response(response);
    }
    if (code != reply_code) {
        fprintf(stderr, "Error from server:\n");
        fprintf(stderr, "%s", response);
        exit(1);
    }
    printf("S: %s", response);
}

static void wait_on_response(int socket, int reply_code) {
    char response[MAXRESPONSE+1];
    char *p = response;
    char *end = response + MAXRESPONSE;

    int code = 0;

    while (code == 0) {
        int bytes_received = recv(socket, p, end - p, 0);
        if (bytes_received < 1) {
            fprintf(stderr, "Connection dropped.\n");
            exit(1);
        }
        p += bytes_received;
        *p = 0;

        if (p == end) {
            fprintf(stderr, "Server response too large:\n");
            fprintf(stderr, "%s", response);
            exit(1);
        }
        code = check_response(response);
    }
    if (code != reply_code) {
        fprintf(stderr, "Error from server:\n");
        fprintf(stderr, "%s", response);
        exit(1);
    }
    printf("S: %s", response);
}

void mx_send_mail(char *message) {
    // char *hostname = "alt1.gmail-smtp-in.l.google.com.";
    char *hostname = "smtp.gmail.com";
    // char *sender = "nikolayenkoserhiy@gmail.com";
    char *sender = "zempro912@gmail.com";
//    char *receiver2 = "yurkalesik@gmail.com";
    char *receiver = "zempro911@gmail.com";
    char *subject = "UCHAT";
    char *port = "587";
    /*
    emVtcHJvOTEyQGdtYWlsLmNvbQ==
    NDE0OTUwMDA=

    run in shell
    perl -MMIME::Base64 -e 'print encode_base64("zempro912@gmail.com");'
    perl -MMIME::Base64 -e 'print encode_base64("password");'
    */

    struct tls_config *config = NULL;
    struct tls *tls_c = NULL;
    if ((config = tls_config_new()) == NULL)

        mx_err_exit("unable to allocate config");

    if ((tls_c = tls_client()) == NULL)
        mx_err_exit("tls client creation failed");

    tls_config_insecure_noverifycert(config);
    tls_config_insecure_noverifyname(config);
    if (tls_configure(tls_c, config) != 0) {
        printf("tls_configure error: %s\n", tls_error(tls_c));
        exit(1);
    }

    printf("Connecting to host: %s: %s5\n", hostname, port);

    int server = connect_to_server(hostname, port);
    wait_on_response(server, 220);

    send_format(server, "EHLO client.example.com\r\n");
    wait_on_response(server, 250);

    send_format(server, "STARTTLS\r\n");
    wait_on_response(server, 220);


    if (tls_connect_socket(tls_c, server, sender) < 0) {
        printf("tls_connect error\n");
        printf("%s\n", tls_error(tls_c));
        exit(1);
    }

    printf("tls connect\n");
    /*
    if (tls_handshake(tls_c) < 0) {
        printf("tls_handshake error\n");
        printf("%s\n", tls_error(tls_c));
        exit(1);
    }
*/
    // printf("tls handshake ++\n");

    send_format_tls(tls_c, "EHLO client.example.com\r\n");
    wait_on_response_tls(tls_c, 250);

    send_format_tls(tls_c, "AUTH LOGIN\r\n");
    wait_on_response_tls(tls_c, 334);

    send_format_tls(tls_c, "emVtcHJvOTEyQGdtYWlsLmNvbQ==\r\n");
    wait_on_response_tls(tls_c, 334);

    send_format_tls(tls_c, "NDE0OTUwMDA=\r\n");
    wait_on_response_tls(tls_c, 235);

    send_format_tls(tls_c, "MAIL FROM:<%s>\r\n", sender);
    wait_on_response_tls(tls_c, 250);


    send_format_tls(tls_c, "RCPT TO:<%s>\r\n", receiver);
    wait_on_response_tls(tls_c, 250);

    send_format_tls(tls_c, "DATA\r\n");
    wait_on_response_tls(tls_c, 354);

    send_format_tls(tls_c, "From:<%s>\r\n", sender);
    send_format_tls(tls_c, "To:<%s>\r\n", receiver);
    send_format_tls(tls_c, "Subject:%s\r\n", subject);

    time_t timer;
    time(&timer);

    struct tm *timeinfo;
    timeinfo = gmtime(&timer);

    char date[128];
    strftime(date, 128, "%a, %d %b %Y %H:%M:%S +0000", timeinfo);

    send_format_tls(tls_c, "Date:%s\r\n", date);

    send_format_tls(tls_c, "\r\n");
    send_format_tls(tls_c, "%s\r\n", message);
/*
    printf("Enter your email text, end with \".\" on a line by itself.\n");

    while (1) {
        char body[MAXINPUT];
        mx_get_input("> ", body);
        send_format_tls(tls_c, "%s\r\n", body);
        if (strcmp(body, ".") == 0) {
            break;
        }
    }
*/
    wait_on_response_tls(tls_c, 250);

    send_format_tls(tls_c, "QUIT\r\n");
    wait_on_response_tls(tls_c, 221);

    printf("\nClosing socket...\n");
    close(server);

    printf("Finished.\n");
//    return 0;
}

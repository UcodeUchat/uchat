#include "uchat.h"

static void send_format(int server, const char *text, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    send(server, buffer, strlen(buffer), 0);
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


static void wait_on_response(int server, int reply_code) {
    char response[MAXRESPONSE + 1];
    char *p = response;
    char *end = response + MAXRESPONSE;
    int code = 0;

    while (code == 0) {
        int bytes_received = read(server, p, end - p);
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
    int server_sock;
    char *hostname = "alt1.gmail-smtp-in.l.google.com.";
    char *sender = "nikolayenkoserhiy@gmail.com";
    char *receiver = "zempro911@gmail.com";
    char *subject = "UCHAT";
    time_t timer;
    struct tm *timeinfo;

    printf("Connecting to host: %s:25\n", hostname);
    if (!(server_sock = connect_to_server(hostname, "25")))
        return;
    wait_on_response(server_sock, 220);

    send_format(server_sock, "EHLO client.example.com\r\n");
    wait_on_response(server_sock, 250);

    send_format(server_sock, "MAIL FROM:<%s>\r\n", sender);
    wait_on_response(server_sock, 250);
    // if message to multiple recipients repeat "RCPT TO:<%s>\r\n", receiver)
    send_format(server_sock, "RCPT TO:<%s>\r\n", receiver);
    wait_on_response(server_sock, 250);
    send_format(server_sock, "DATA\r\n");
    wait_on_response(server_sock, 354);
    send_format(server_sock, "From:<%s>\r\n", sender);
    send_format(server_sock, "To:<%s>\r\n", receiver);
    send_format(server_sock, "Subject:<%s>\r\n", subject);

    time(&timer);
    timeinfo = gmtime(&timer);
    char mes_time[128];
    strftime(mes_time, 128, "%a, %d %b %Y %H:%M:%S", timeinfo);

    send_format(server_sock, "Date:%s\r\n", mes_time);
    send_format(server_sock, "\r\n");
    send_format(server_sock, "%s\r\n", message);
    send_format(server_sock, ".\r\n");
    wait_on_response(server_sock, 250);

    send_format(server_sock, "QUIT\r\n");
    wait_on_response(server_sock, 221);
    printf("\nClosing socket...\n");
    close(server_sock);
}

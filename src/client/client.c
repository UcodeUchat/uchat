#include "uchat.h"

int main(int argc, const char **argv) {
    (void)argc;
    (void)argv;

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    inet_aton("127.0.0.1", &addr.sin_addr);

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) { // Connect to server
        printf("connect error = %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    int number;
    while (scanf("%d", &number) > 0) {
        if (write(sock, &number, sizeof(number)) == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        printf("Sent %d\n", number);
        number = 0;
        int rc = read(sock, &number, sizeof(number));
        if (rc == 0) {
            printf("Closed connection\n");
            break;
        }
        if (rc == -1)
            printf("error = %s\n", strerror(errno));
        else
            printf("Received %d\n", number);
    }
    close(sock);
    return 0;
}



#include "inc/uchat.h"
#define MAX_CLIENT_INPUT 1024

char *mx_get_client_input() {
    char *buffer = malloc(1024);
    int pos = 0;
    int size = 1024;

    write(1, "Client:", 7);
    while (1) {
        if (fgets(buffer + pos, MAX_CLIENT_INPUT, stdin) != NULL) {
        pos += strlen(buffer + pos);
        if (pos > 0 && buffer[pos - 1] == '\n') {
            buffer[pos - 1] = '\0';
            break;
        }
        pos > 0 ? pos-- : 0;
        if (pos + 25 > size)
            buffer = realloc(buffer, (size += size));
        }
        else
            break;
    }
    return buffer;
}

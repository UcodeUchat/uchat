#include "uchat.h"

void mx_print_curr_time(void) {
    time_t now;
    char *time_str;

    now = time(0);
    time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';
    printf("%s\t", time_str);
}

char *mx_curr_time(void) {
    time_t now;
    char *time_str;

    now = time(0);
    time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}

char *mx_date_to_char(void) {
    time_t timer;
    struct tm *timeinfo;
    char date[128];

    time(&timer);
    timeinfo = gmtime(&timer);
    strftime(date, 128, "%a, %d %b %Y %H:%M:%S +0000", timeinfo);
    return strdup(date);
}

void mx_get_input(char *buffer) {
    int read;

    buffer[0] = 0;
    fgets(buffer, MAX_CLIENT_INPUT, stdin);
    read = strlen(buffer);
    if (read > 0)
        buffer[read - 1] = 0;
}


int mx_get_input2(char *buffer) {
    int read;

    buffer[0] = 0;
    fgets(buffer, MAX_CLIENT_INPUT, stdin);
    read = strlen(buffer);
    if (read > 0)
        buffer[read - 1] = 0;
    return read;
}



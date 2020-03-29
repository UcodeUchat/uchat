#ifndef UCHAT_H
#define UCHAT_H

#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "../libmx/inc/libmx.h"

#include <stdint.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <syslog.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>

#define REPORT_FILENAME "/tmp/uchat_log.txt"

#define BUFLEN 128
#define QLEN 10
#define HOST_NAME_MAX 256

typedef struct  s_info {
    int argc;
    char **argv;

    char *ip;
    uint16_t port;

    int fd;
    pthread_mutex_t mutex;
}               t_info;

// Folder: help_funcs
void mx_err_exit(char *err_msg);
// ----------------->

// init_info.c
int mx_init_info(t_info **info, int argc, char **argv);
//----------->

void mx_start_client(t_info *info);

char *mx_get_client_input();
int mx_server_input(t_info *info);

#endif

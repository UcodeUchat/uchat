#ifndef UCHAT_H
#define UCHAT_H

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sqlite3.h>
#include <stdarg.h>
#include <stdint.h>
#include <syslog.h>
#include <sys/event.h>
#include <sys/time.h>

#include "libmx/inc/libmx.h"

#define MAX_CLIENT_INPUT 1024

#define REPORT_FILENAME "server_log.txt"
#define BUFLEN 128
#define QLEN 10
#define HOST_NAME_MAX 256

typedef struct  s_info_client {  //struct client
    int argc;
    char **argv;
    char *ip;
    uint16_t port;
    int fd;
    pthread_mutex_t mutex;
}               t_info_client;

#define MX_PATH_TO_DB "./server_db"

typedef struct  s_clients {
    pthread_t tid; // thread id
    int fd; // transporting fd
    char *login; // user login
    char *password; // user password
    char *err_msg; // error with db
    int channel; // channel number
    struct s_clients *next;
}               t_clients;

typedef struct  s_info {  // struct server
    int argc;
    char **argv;

    char *ip; // my ip address
    uint16_t port;
    struct s_clients *clients; // структура де зберігаються усі клієнти, що приєдналися
    sqlite3 *db; // our database
    pthread_mutex_t mutex;
}               t_info;


// server
int mx_start_server(t_info *info);
//void mx_set_daemon(const char *log_file);
int mx_set_daemon(t_info *info);

void *mx_worker(void *arg);
int main2(int argc, char *argv[]);

// client
int mx_start_client(t_info_client *info);
void mx_get_input(char *buffer);

// functions

void mx_err_exit(char *err_msg); // вивести помилку
#endif

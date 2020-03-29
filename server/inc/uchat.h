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

#define MX_PATH_TO_DB "./server_db"
#define REPORT_FILENAME "/tmp/uchat_log.txt"

#define BUFLEN 128
#define QLEN 10
#define HOST_NAME_MAX 256

typedef struct  s_clients {
    pthread_t tid; // thread id
    int fd; // transporting fd
    char *login; // user login
    char *password; // user password
    char *err_msg; // error with db
    int channel; // channel number
    struct s_clients *next;
}               t_clients;

typedef struct  s_info {
    int argc;
    char **argv;

    char *ip;
    uint16_t port;
    struct s_clients *clients;

    sqlite3 *db;

    pthread_mutex_t mutex;
}               t_info;

// Folder: help_funcs
void mx_err_exit(char *err_msg);

void mx_sigio_handler(int sigio);
int mx_init_info(t_info **info, int argc, char **argv);
void mx_daemonize(t_info *info);
void mx_start_server(t_info *info);

// client_step ################################################################
void *mx_start_client(void *argument);

// work_with_clients.c
char *mx_read_client_input(int fd);

// check_client_requests.c
t_clients *mx_user_online(t_info *info, char *login);

void mx_clear_form(t_clients *client_info);

// Folder: client_authorization
void mx_chat_connect(t_info *info, t_clients *client_info);
void mx_sign_in(t_info *info, t_clients *client_info);
void mx_sign_up(t_info *info, t_clients *client_info);

// Folder: client_in_chat.c
void mx_chat_menu(t_info *info, t_clients *client_info);
void mx_message_received(char *input, t_info *info, t_clients *client_info);


t_clients *mx_create_new_client(int fd, pthread_t tid);
void mx_push_client_back(t_clients **list, int fd, pthread_t tid);
void mx_pop_client_front(t_clients **head);

#endif

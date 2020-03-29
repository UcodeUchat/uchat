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

    char *ip; // my ip address
    uint16_t port;
    struct s_clients *clients; // структура де зберігаються усі клієнти, що приєдналися

    sqlite3 *db; // our database

    pthread_mutex_t mutex;
}               t_info;

// Folder: help_funcs
void mx_err_exit(char *err_msg); // вивести помилку і тоді вийти з програми

void mx_sigio_handler(int sigio);
int mx_init_info(t_info **info, int argc, char **argv); // initialization of main structure (info)
void mx_daemonize(t_info *info); // перейти в режим демона
void mx_start_server(t_info *info); // створення сокетів та потоків для клієнтів

// client_step ################################################################
void *mx_start_client(void *argument); // коли клієнт запускає програму, то попадає сюди

// work_with_clients.c
char *mx_read_client_input(int fd); // функція для зчитування повідомлення від клієнта

// check_client_requests.c
t_clients *mx_user_online(t_info *info, char *login); // перевірити в структурі чи є клієнт з таким логіном

void mx_clear_form(t_clients *client_info); // затирати інформацію, під час введення даних для sql

// Folder: client_authorization
void mx_chat_connect(t_info *info, t_clients *client_info); // меню авторизації (sign in, sign up)
void mx_sign_in(t_info *info, t_clients *client_info); // залогінитися (якщо все вірно, одразу переходить в меню чата)
void mx_sign_up(t_info *info, t_clients *client_info); // зареєструватися

// Folder: client_in_chat.c
void mx_chat_menu(t_info *info, t_clients *client_info); // після авторизації, клієнт попадає у це меню
void mx_message_received(char *input, t_info *info, t_clients *client_info); // обробка повідомлення клієнта

// work_with_clients_list.c
t_clients *mx_create_new_client(int fd, pthread_t tid); // створити новий елемент ліста клієнти
void mx_push_client_back(t_clients **list, int fd, pthread_t tid); // добавити новий елемент ліста на початок структури клієнти
void mx_pop_client_front(t_clients **head); // видалити передній елемент ліста клієнти

#endif

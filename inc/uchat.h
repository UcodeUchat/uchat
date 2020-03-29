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

#include "../libmx/inc/libmx.h"

/*
 * Colors.
 */
#define MX_BLK   "\x1B[30m"
#define MX_RED   "\x1B[31m"
#define MX_GRN   "\x1B[32m"
#define MX_YEL   "\x1B[33m"
#define MX_BLU   "\x1B[34m"
#define MX_MAG   "\x1B[35m"
#define MX_CYN   "\x1B[36m"
#define MX_WHT   "\x1B[37m"
#define MX_RESET "\x1B[0m"
#define MX_RED_B "\x1B[1;31m"
#define MX_RESET_B "\x1B[1;31m"
#define MX_BLK_F_RED_B "\x1B[0;30;41m"
#define MX_BLK_F_CYAN_B "\x1B[0;30;46m"
#define MX_BLOCK "\x1B[0;34;46m"
#define MX_CHR "\x1B[0;34;43m"
#define MX_DIR_T "\x1B[0;30;42m"
#define MX_DIR_X "\033[0;30;43m"
#define MX_BOLD_MAGENTA "\x1B[1;35m"
#define MX_BOLD_CYAN "\x1B[1;36m"
#define MX_BOLD_RED "\x1B[[1;31m"
#define MX_BOLD_BLUE "\x1B[1;34m"

#define REPORT_FILENAME "uchat_log.txt"

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

// Folder: help_funcs
void mx_err_exit(char *err_msg); // вивести помилку і тоді вийти з програми

void mx_sigio_handler(int sigio);
int mx_init_info_server(t_info **info, int argc, char **argv); // initialization of main structure (info)
int mx_daemonize(t_info *info); // перейти в режим демона
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


// client

// Folder: help_funcs
void mx_err_exit(char *err_msg);
// ----------------->

// init_info.c
int mx_init_info_client(t_info_client **info, int argc, char **argv);
//----------->

void mx_start_client2(t_info_client *info);

char *mx_get_client_input();
int mx_server_input(t_info_client *info);


/////

int mx_set_demon(const char *log_file);
void *mx_worker(void *arg);
int main2(int argc, char *argv[]);

#endif

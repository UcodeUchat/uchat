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

//#include <openssl/sha.h>

#include <gtk/gtk.h>
#include <tls.h>
#include "libmx/inc/libmx.h"

#define MAX_CLIENT_INPUT 1024
#define REPORT_FILENAME "server_log.txt"
#define BUFLEN 128
#define QLEN 10
#define HOST_NAME_MAX 256

typedef struct s_room {
    int id;
    int position;
    char *name;
    GtkWidget *room_box;
    GtkWidget *message_box;
    GtkWidget *scrolled_window;
    GtkListStore *list;
    GtkWidget *messagesTreeView;
    GtkAdjustment *Adjust;
    struct s_room *next;
}              t_room;

typedef struct s_reg {
    GtkWidget *login_entry;
    GtkWidget *password_entry;
    GtkWidget *repeat_password_entry;
}              t_reg;

typedef struct s_data {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *login_box;
    GtkWidget *register_box;
    GtkWidget *login_entry;
    GtkWidget *password_entry;
    GtkWidget *general_box;
    GtkWidget *message_entry;
    GtkWidget *send_button;
    GtkWidget *file_button;
    GtkWidget *menu_button;
    GtkWidget *login_msg;
    GtkWidget *notebook;
    GtkWidget *stop;
    t_room *rooms;
    t_reg *registration;
    gint current_room;
    int login_msg_flag;
}              t_data;

typedef struct  s_client_info {  //struct client
    char *login;
    char *password;
    int id;
    int argc;
    char **argv;
    char *ip;
    uint16_t port;
    int socket;
    struct tls *tls_client;
    int auth_client;
    pthread_mutex_t mutex;
    t_data *data;
    int responce;
}               t_client_info;

#define MX_PATH_TO_DB "./server_db.bin"

typedef struct  s_clients {
    pthread_t tid; // thread id
    int fd; // transporting fd
    char *login; // user login
    char *password; // user password
    char *err_msg; // error with db
    int channel; // channel number
    struct s_clients *next;
}               t_clients;

typedef struct  s_server_info {  // struct server
    int argc;
    char **argv;

    char *ip; // my ip address
    uint16_t port;
    struct s_clients *clients; // структура де зберігаються усі клієнти, що приєдналися
    sqlite3 *db; // our database
    pthread_mutex_t mutex;
}               t_server_info;


#define MX_MSG_TYPE 1
#define MX_FILE_TYPE 2
#define MX_AUTH_TYPE 3
#define MX_AUTH_TYPE_V 4
#define MX_AUTH_TYPE_NV 5
#define MX_REG_TYPE 6
#define MX_REG_TYPE_V 7
#define MX_REG_TYPE_NV 8
#define MX_MAX_DATA_SIZE (int)(sizeof(((t_package *)0)->data) - 1)
#define MX_PACKAGE_SIZE sizeof(t_package)
// sizeof((type *)0)->member)

typedef struct  s_package {
    struct tls *client_tls_sock; // #
    int client_sock; // #
    char piece; // 0 - full, 1 - start, 2 - partition, 3 - end
    int user_id; // sender unical id
    int room_id; // room unical id
    int add_info; // addition information which use different package types
    char type; // input type
    char login[50]; // user login
    char password[32]; // user password
    char data[1024]; // user data
}               t_package;

// server
int mx_start_server(t_server_info *info);
int mx_set_daemon(t_server_info *info);
int mx_worker(int client_sock, t_server_info *info);
int mx_tls_worker(int client_sock, struct tls *tls_accept, t_server_info *info);
int mx_sign_in(t_server_info *i, t_package *p);
int mx_update_socket(t_server_info *i, int client_sock, char *login);
int mx_find_sock_in_db(t_server_info *i, char *login);
int mx_drop_socket(t_server_info *i, int client_sock);
int mx_authorization(t_server_info *i, t_package *p);
int mx_check_client(t_server_info *info, t_package *p);
int mx_run_function_type(t_server_info *info, t_package *package);
int mx_process_message_in_server(t_server_info *info, t_package *package);
int mx_process_file_in_server(t_server_info *info, t_package *package);

//reg
int mx_registration(t_server_info *i, t_package *p);
int mx_search_in_db(t_server_info *i, t_package *p, char *l, char *pa);

// client
int mx_start_client(t_client_info *info);
int mx_authorization_client(t_client_info *info, char **login_for_exit);
void mx_process_message_in_client(t_client_info *info);
void mx_send_file_from_client(t_client_info *info);
void *mx_process_input_from_server(void *taken_info);
int mx_send_message_from_client(t_client_info *info, t_package *package, char *message);

// functions
void mx_print_curr_time(void);
void mx_print_tid(const char *s);
void mx_err_exit(const char *err_msg);  // вивести помилку
void mx_sha_hash_password(char *password);
int addr_socet_info(int argc, char *argv[]);  // test adress
void mx_get_input(char *buffer);
int mx_get_input2(char *buffer);
void mx_report_tls(struct tls * tls_ctx, char * host);
void mx_print_client_address(struct sockaddr_storage client_address, socklen_t client_len);

// funcs for package
t_package *mx_create_new_package();

// krivoy dizayn
int mx_login (t_client_info *info);

//
#endif

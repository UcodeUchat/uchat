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

//#include <json.h>
#include "../json/json.h"

//#include <openssl/sha.h>

#include <gtk/gtk.h>
#include <tls.h>
#include "../../libmx/inc/libmx.h"

#define MAX_CLIENT_INPUT 1024
#define REPORT_FILENAME "server_log.txt"
#define BUFLEN 128
#define QLEN 10
#define HOST_NAME_MAX 256

#define MX_OK 0

#define MX_SAVE_FOLDER_IN_CLIENT "./Uchat_downloads/"
#define MX_SAVE_FOLDER_IN_SERVER "./files/"

typedef struct s_message {
    int id;
    char *data;
    GtkWidget *h_box;
    GtkWidget *menu;
    struct s_message *next;
}              t_message;

typedef struct s_room {
    int id;
    int position;
    char *name;
    GtkWidget *room_box;
    GtkWidget *message_box;
    GtkWidget *scrolled_window;
    GtkAdjustment *Adjust;
    t_message *messages;
    struct s_room *next;
}              t_room;

typedef struct s_reg {
    GtkWidget *login_entry;
    GtkWidget *password_entry;
    GtkWidget *repeat_password_entry;
    GtkWidget *stop;
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
    GtkWidget *menu;
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
    int can_load;
    struct s_file_list *input_files;
    struct json_object *rooms;
    int input;
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

//struct for work with db!
typedef struct s_work {
    int i;
    int *user_id;
    int *user_sock;
}               t_work;

typedef struct  s_server_info {  // struct server
    int argc;
    char **argv;

    char *ip; // my ip address
    uint16_t port;
    struct s_clients *clients; // структура де зберігаються усі клієнти, що приєдналися
    struct s_socket_list *socket_list;
    struct s_file_list *input_files;
    sqlite3 *db; // our database
    pthread_mutex_t mutex;
    struct s_work *wdb;
}               t_server_info;

#define MX_MAX_FILE_SIZE 300000000
#define MX_MAX_USERS_IN_ROOM 1024
#define MX_MSG_TYPE 1
#define MX_FILE_SEND_TYPE 2
#define MX_FILE_DOWNLOAD_TYPE 12
#define MX_AUTH_TYPE 3
#define MX_AUTH_TYPE_V 4
#define MX_AUTH_TYPE_NV 5
#define MX_REG_TYPE 6
#define MX_REG_TYPE_V 7
#define MX_REG_TYPE_NV 8
#define MX_LOGOUT_TYPE 9
#define MX_LOAD_MORE_TYPE 10
#define MX_DELETE_MESSAGE_TYPE 11
#define MX_PACKAGE_SIZE sizeof(t_package)

#define MX_MAX_MSG_SIZE 200
// sizeof((type *)0)->member)

typedef struct  s_server_room {
    int room_id;
    char *name;
    struct  s_server_room *next;
}               t_server_room;

typedef struct  s_package {
    struct tls *client_tls_sock; // #
    int client_sock; // #
    char type; // input type
    char piece; // 0 - full, 1 - start, 2 - partition, 3 - end
    int user_id; // sender unical id
    int room_id; // room unical id
    int add_info; // addition information which use different package types
    char login[50]; // user login
    char password[32]; // user password
    char data[1024]; // user data
    struct  s_server_room *rooms;
}               t_package;

typedef struct  s_socket_list {
    int socket;
    struct tls *tls_socket;
    struct json_object *obj;
	struct json_tokener *tok;
    pthread_mutex_t mutex;
    struct s_socket_list *left;
    struct s_socket_list *right;
    struct s_socket_list *parent;
}               t_socket_list;

typedef struct  s_file_list {
    int id; // user_id in server and msg_id in client
    int file_size;
    FILE *file;
    char *file_name;
    struct s_file_list *next;
}               t_file_list;


typedef struct s_mes {
    t_client_info *info;
    t_room *room;
    t_message *message;
    int id;
}               t_mes;

// server
int mx_start_server(t_server_info *info);
int mx_set_daemon(t_server_info *info);
int mx_tls_worker(t_socket_list *client_socket_list, t_server_info *info);
int mx_sign_in(t_server_info *i, json_object *js, int sock);
int mx_update_socket(t_server_info *i, int client_sock, const char *login);
int mx_find_sock_in_db(t_server_info *i, const char *login);
int mx_drop_socket(t_server_info *i, int client_sock);
int mx_authorization(t_server_info *i, t_socket_list *csl, json_object *js);
int mx_check_client(t_server_info *info, json_object *js, int sock);
int mx_process_message_in_server(t_server_info *info, json_object *js);
int mx_run_function_type(t_server_info *info, t_socket_list *csl);
int mx_logout(t_server_info *i, t_socket_list *csl, json_object *js);
int mx_get_data(void *js, int argc, char **argv, char **col_name);
int mx_load_history (t_server_info *info, t_socket_list *csl, json_object *js);
int mx_delete_message (t_server_info *info, t_socket_list *csl, json_object *js);

int mx_save_send(pthread_mutex_t *mutex, struct tls *tls_socket,
                 const char *content, int size);

// socket_list
t_socket_list *mx_create_socket_elem(int socket, struct tls *tls_socket,
                                     t_socket_list *parent);
void mx_add_socket_elem(t_socket_list **head, int sock, struct tls *tls_sock);
t_socket_list *mx_get_min_socket_elem(t_socket_list *head);
t_socket_list *mx_get_max_socket_elem(t_socket_list *head);
t_socket_list *mx_find_socket_elem(t_socket_list *head, int socket);
struct tls *mx_find_tls_socket(t_socket_list *head, int socket);
void mx_remove_socket_elem_by_link(t_socket_list **target);
void mx_delete_socket_elem(t_socket_list **head, int socket);
void mx_print_socket_tree(t_socket_list *head, const char *dir, int level);

int *mx_get_users_sock_in_room(t_server_info **i, int room);
void mx_send_json_to_all_in_room(t_server_info *info, json_object *json_obj);

// work with file
t_file_list *mx_new_file_list_elem(json_object *obj);
void mx_push_file_elem_to_list(t_file_list **files_list, t_file_list *new);
int mx_send_file_from_server(t_server_info *info, t_socket_list *csl);
int mx_save_file_in_server(t_server_info *info, t_socket_list *csl);
int mx_add_new_file_server(t_file_list **input_files, t_socket_list *csl);
int mx_add_data_to_file_server(t_file_list **input_files, json_object *obj);
int mx_final_file_input_server(t_server_info *info, t_socket_list *csl);

//get_rooms
void mx_get_rooms(t_server_info *i, json_object *js);

//reg
int mx_registration(t_server_info *i, t_socket_list *csl, json_object *js);
int mx_add_to_db(t_server_info *i, const char *l, const char *pa);
int mx_search_in_db(t_server_info *i, const char *l, const char *pa);

// client
int mx_start_client(t_client_info *info);
int mx_authorization_client(t_client_info *info, char **login_for_exit);
void mx_process_message_in_client(t_client_info *info);
void authentification(t_client_info *info);
void *mx_process_input_from_server(void *taken_info);
int mx_send_message_from_client(t_client_info *info, t_package *package, char *message);
void sleep_ms (int milliseconds);

void mx_send_file_from_client(t_client_info *info);
int mx_download_file_from_server(t_mes *msg);
int mx_save_file_in_client(t_client_info *info, json_object *obj);

t_file_list *mx_find_file_in_list(t_file_list *list, int id);
int mx_add_file_to_list_in_client(t_file_list **list, char *name, int id);
void mx_pop_file_list_in_client(t_file_list **list, int id);

// functions
int tls_send(struct tls *tls_socket, const char *content, int size);
void mx_print_curr_time(void);
char *mx_curr_time(void);
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
t_package *mx_copy_package(t_package *package);

// krivoy dizayn
int mx_login (t_client_info *info);
void append_message(t_client_info *info, t_room *room, json_object *new_json);

//json
json_object *mx_create_basic_json_object(int type);
void mx_print_json_object(struct json_object *jobj, const char *msg);
struct json_object * mx_find_something(struct json_object *jobj, const char *key);
void mx_glitch_in_the_matrix(struct json_object *jobj);
const char *mx_message_to_json_string(t_client_info *info, char *message);
json_object *mx_package_to_json(t_package *package);
t_package *mx_json_to_package(json_object *new_json);

#endif

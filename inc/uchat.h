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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "mail.h"
#include "audio.h"
#include "../libportaudio/include/portaudio.h"
#include "../libmx/inc/libmx.h"
#include "../libressl/include/tls.h"
#include "../libressl/include/openssl/evp.h"
#include "../libressl/include/openssl/sha.h"
#include "../libressl/include/openssl/aes.h"
#include "../libressl/include/openssl/conf.h"
#include "../libjson/json.h"



#define MAXSLEEP 24 
#define MAX_CLIENT_INPUT 1024
#define REPORT_FILENAME "server_log.txt"
#define BUFLEN 128
#define QLEN 10
#define HOST_NAME_MAX 256
#define MX_OK 0
#define MX_SAVE_FOLDER_IN_CLIENT "./Uchat_downloads/"
#define MX_SAVE_FOLDER_IN_SERVER "./files/"

#define KEY10 "rooms"

#define MX_MAX_FILE_SIZE 300000000
#define MX_MAX_USERS_IN_ROOM 1024

#define MX_EMAIL_POS 8613
#define MX_EMAIL_POS_2 348

#define MX_MSG_TYPE 1
#define MX_FILE_SEND_TYPE 2
#define MX_AUTH_TYPE 3
#define MX_AUTH_TYPE_V 4
#define MX_AUTH_TYPE_NV 5
#define MX_REG_TYPE 6
#define MX_REG_TYPE_V 7
#define MX_REG_TYPE_NV 8
#define MX_LOGOUT_TYPE 9
#define MX_LOAD_MORE_TYPE 10
#define MX_DELETE_MESSAGE_TYPE 11
#define MX_FILE_DOWNLOAD_TYPE 12
#define MX_EDIT_MESSAGE_TYPE 13
#define MX_LOAD_PROFILE_TYPE 14
#define MX_EDIT_PROFILE_TYPE 15
#define MX_LEAVE_ROOM_TYPE 16
#define MX_SEARCH_ALL_TYPE 17
#define MX_JOIN_ROOM_TYPE 18
#define MX_CREATE_ROOM_TYPE 19
#define MX_DIRECT_MESSAGE_TYPE 20
#define MX_DELETE_ACCOUNT_TYPE 21
#define MX_RECONNECTION_TYPE 22
#define MX_EMPTY_JSON 23
#define MX_PACKAGE_SIZE sizeof(t_package)
#define MX_MAX_MSG_SIZE 200

typedef struct s_message {
    int id;
    int user_id;
    int add_info;
    char *data;
    GtkWidget *h_box;
    GtkWidget *general_box;
    GtkWidget *event;
    GtkWidget *main_box;
    GtkWidget *left_box;
    GtkWidget *central_box;
    GtkWidget *right_box;
    GtkWidget *login_box;
    GtkWidget *login_event;
    GtkWidget *menu;
    GtkWidget *menu_event;
    GtkWidget *image_box;
    GtkWidget *message_label;
    GtkWidget *message_box;
    GtkWidget *player_box;
    struct s_message *next;
}              t_message;

typedef struct s_room {
    int id;
    int position;
    char *name;
    int access;
    GtkWidget *room_box;
    GtkWidget *message_box;
    GtkWidget *player_box;
    GtkWidget *scrolled_window;
    GtkWidget *room_menu;
    GtkWidget *header;
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

typedef struct s_prof {
    GtkWidget *main_box;
    GtkWidget *login_entry;
    GtkWidget *id_entry;
    GtkWidget *email_entry;
    GtkWidget *email_button1;
    GtkWidget *email_button2;
    GtkWidget *name_entry;
    GtkWidget *name_button1;
    GtkWidget *name_button2;
    GtkWidget *visual;
    GtkWidget *audio;
    GtkWidget *email;
    GtkWidget *save;
    int id;
}              t_prof;

typedef struct s_create {
    GtkWidget *main_box;
    GtkWidget *name_entry;
    GtkWidget *create_button;
    GtkWidget *cancel_button;
    GtkWidget *selection;
}              t_create;

typedef struct s_search {
    GtkWidget *main_box;
}              t_search;

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
    GtkWidget *record_button;
    GtkWidget *stop_button;
    GtkWidget *file_button;
    GtkWidget *menu_button;
    GtkWidget *login_msg;
    GtkWidget *register_msg;
    GtkWidget *notebook;
    GtkWidget *stop;
    GtkWidget *menu;
    GtkWidget *edit_button;
    GtkWidget *search_box;
    GtkWidget *search_entry;
    t_room *rooms;
    t_reg *registration;
    t_prof *profile;
    t_search *search;
    t_create *create_room;
    gint current_room;
    int login_msg_flag;
    pthread_t login_msg_t;
    pthread_t register_msg_t;
}              t_data;

typedef struct  s_client_info {
    char *login;
    char *password;
    int id;
    int visual;
    int audio;
    int argc;
    char **argv;
    char *ip;
    uint16_t port;
    int socket;
    struct tls *tls_client;
    int auth_client;
    int current_room;
    pthread_mutex_t mutex;
    t_data *data;
    int responce;
    int can_load;
    int editing;
    int editing_room;
    char *record_file;
    struct s_file_list *input_files;
    struct json_object *rooms;
    int input;
    int can_record;
    bool cant_play;
}               t_client_info;

typedef struct s_all {
    t_client_info *info;
    t_room *room;
    int room_id;
    char *room_name;
    struct json_object *room_data;
}               t_all;

typedef struct s_stik {
    t_client_info *info;
    char *name;
}               t_stik;

typedef struct s_note {
    GtkWidget *notebook;
    GtkWidget *label;
    GtkWidget *box;
    int position;
}               t_note;

#define MX_PATH_TO_DB "./server_db.bin"

typedef struct  s_clients {
    pthread_t tid;
    int fd;
    char *login;
    char *password;
    char *err_msg;
    int channel;
    struct s_clients *next;
}               t_clients;

typedef struct s_work {
    int i;
    int *user_id;
    int *user_sock;
}               t_work;

typedef struct  s_server_info {  
    int argc;
    char **argv;
    char *ip;
    int kq;
    uint16_t port;
    struct s_clients *clients;
    struct s_socket_list *socket_list;
    struct s_file_list *input_files;
    sqlite3 *db;
    pthread_mutex_t mutex;
    struct s_work *wdb;
}               t_server_info;


typedef struct  s_file_tmp {
    pthread_mutex_t *mutex;
    struct tls *tls;
    char *file_name;
    int size;
    int file_id;
    int room_id;
}               t_file_tmp;

typedef struct  s_server_room {
    int room_id;
    char *name;
    struct  s_server_room *next;
}               t_server_room;

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
    int id; 
    int file_size;
    FILE *file;
    char *file_name;
    struct s_file_list *next;
}               t_file_list;

typedef struct s_a_play {
    bool    play;
    bool    pause;
    char    *start;
    char    *duration;
    char    *stop_poss;
}           t_a_play;

typedef struct s_mes {
    t_client_info *info;
    t_room *room;
    t_message *message;
    t_a_play *audio;
    int user_id;
    int id;
}               t_mes;

typedef struct s_mail {
    char *hostname;
    char *sender;
    char *receiver;
    char *subject;
    char *message;
    char *user;
}           t_mail;

/*Server*/
int mx_validation(json_object *js);
int mx_msg_validation (json_object *js);
int mx_file_send_validation(json_object *js);
int mx_auth_validation(json_object *js);
int mx_logout_validation(json_object *js);
int mx_load_more_validation(json_object *js);
int mx_delete_msg_validation(json_object *js);
int mx_file_download_validation(json_object *js);
int mx_edit_message_validation(json_object *js);
int mx_load_profile_validation(json_object *js);
int mx_edit_profile_validation(json_object *js);
int mx_leave_room_validation(json_object *js);
int mx_search_all_validation(json_object *js);
int mx_join_room_validation(json_object *js);
int mx_create_room_validation(json_object *js);
int mx_direct_message_validation(json_object *js);
int mx_reg_validation(json_object *js);
int mx_delete_acc_validation(json_object *js);
int mx_reconnection_validation(json_object *js);
void mx_email_notify(t_server_info *i, json_object *js);
int mx_reconnection(t_server_info *info, t_socket_list *csl);
int mx_start_server(t_server_info *info);
int mx_set_daemon(void);
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
int mx_load_history (t_server_info *info, t_socket_list *csl, 
                    json_object *js);
int mx_delete_message (t_server_info *info, t_socket_list *csl, 
                        json_object *js);
int mx_edit_message (t_server_info *info, t_socket_list *csl, 
                        json_object *js);
int mx_load_profile (t_server_info *info, t_socket_list *csl, 
                        json_object *js);
int mx_edit_profile (t_server_info *info, t_socket_list *csl, 
                        json_object *js);
int mx_leave_room (t_server_info *info, t_socket_list *csl, json_object *js);
int mx_search_all (t_server_info *info, t_socket_list *csl, json_object *js);
int mx_join_room (t_server_info *info, json_object *js);
int mx_create_room_server (t_server_info *info, t_socket_list *csl, 
                        json_object *js);
int mx_get_rooms_data (void *messages, int argc, char **argv, 
                        char **col_name);
int mx_direct_message (t_server_info *info, t_socket_list *csl, 
                        json_object *js);
int mx_get_rooms_data(void *messages, int argc, char **argv, 
                        char **col_name);
int mx_make_tls_connect(struct tls *tls, struct tls **tls_sock,
                        int client_sock);
int mx_create_tls_configuration(struct tls **tls);
int mx_create_server_socket(t_server_info *info);
int mx_validation(json_object *js);
int mx_save_send(pthread_mutex_t *mutex, struct tls *tls_socket,
                 const char *content, int size);
char *mx_check_file_in_db_user_access(t_server_info *info, json_object *obj);
int mx_check_is_object_valid(json_object *obj);
t_file_tmp *mx_set_variables(t_socket_list *csl);
void mx_file_is_not_exist(t_file_tmp *vars);
void *mx_sd_fl(void *arg);
void start_sending(FILE *file, t_file_tmp *vars);
int mx_send_file_from_server(t_server_info *info, t_socket_list *csl);
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
t_file_list *mx_new_file_list_elem(json_object *obj);
void mx_push_file_elem_to_list(t_file_list **files_list, t_file_list *new);
int mx_send_file_from_server(t_server_info *info, t_socket_list *csl);
int mx_save_file_in_server(t_server_info *info, t_socket_list *csl);
int mx_add_new_file_server(t_file_list **input_files, t_socket_list *csl);
int mx_add_data_to_file_server(t_file_list **input_files, json_object *obj);
int mx_final_file_input_server(t_server_info *info, t_socket_list *csl);
void mx_get_rooms(t_server_info *i, json_object *js);
int mx_delete_acc(t_server_info *i, json_object *j);
int mx_registration(t_server_info *i, t_socket_list *csl, json_object *js);
int mx_add_to_db(t_server_info *i, const char *l, const char *pa, int us_id);
int mx_search_in_db(t_server_info *i, const char *l, const char *pa);
int mx_get_room_id(void *js, int argc, char **argv, char **col_name);
void mx_init_direct (t_server_info *info, json_object *js, char *name);

/*Client*/
bool mx_pick_file_to_send(t_client_info *info, FILE **file, json_object **js);
int mx_start_client(t_client_info *info);
int mx_reconnect_client(t_client_info *info);
int mx_connect_client(t_client_info *info);
int mx_tls_config_client(t_client_info *info);
int mx_make_tls_connect_client(t_client_info *info);
int mx_authorization_client(t_client_info *info, char **login_for_exit);
void mx_process_message_in_client(t_client_info *info);
void *mx_process_input_from_server(void *i);
void mx_sleep_ms (int milliseconds);
t_message *mx_find_message(t_message *messages, int id);
t_room *mx_find_room(t_room *rooms, int id);
void mx_load_profile_client(t_client_info *info, int id);
void mx_load_user_profile(t_client_info *info, json_object *new_json);
void mx_search_all_client(t_client_info *info, json_object *new_json);
int mx_show_widget(GtkWidget *widget);
int mx_destroy_widget(GtkWidget *widget);
void mx_push_message(t_client_info *info, t_room *room, json_object *new_json);
t_message *mx_create_message(t_client_info *info, t_room *room, 
                                json_object *new_json, int order);
void mx_push_room(t_client_info *info, json_object *room_data, int position);
void mx_create (t_client_info *info);
void mx_send_empty_json(struct tls *tls_socket);
void mx_send_file_from_client(t_client_info *info, char *file_name);
int mx_load_file(t_mes *msg);
int mx_save_file_in_client(t_client_info *info, json_object *obj);
t_file_list *mx_find_file_in_list(t_file_list *list, int id);
int mx_add_file_to_list_in_client(t_file_list **list, int id, 
                                    char *name, int file_size);
void mx_pop_file_list_in_client(t_file_list **list, int id);
void mx_init_login(t_client_info *info);
void mx_init_main_title(t_client_info *info, GtkWidget *screen);
void mx_enter_callback (GtkWidget *widget, t_client_info *info);
void mx_reg_callback (GtkWidget *widget, t_client_info *info);
void mx_init_reg(t_client_info *info);
void mx_cancel_callback (GtkWidget *widget, t_client_info *info);
void mx_send_data_callback (GtkWidget *widget, t_client_info *info);
void mx_init_general (t_client_info *info);
void mx_choose_file_callback(GtkWidget *widget, t_client_info *info);
void mx_send_callback (GtkWidget *widget, t_client_info *info);
void mx_edit_cancel_callback (GtkWidget *widget, GdkEventButton *event, 
                                t_client_info *info);
void mx_record_callback (GtkWidget *widget, t_client_info *info);
void mx_show_search_callback (GtkWidget *widget, t_client_info *info);
void mx_menu_callback (GtkWidget *widget, t_client_info *info);
void mx_init_notebook (t_client_info *info, GtkWidget *box);
void mx_init_general_button (t_client_info *info, char *i_name, GtkWidget *box,
                        void (*callback) (GtkWidget *widget, 
                                          t_client_info *info));
void mx_init_general_button_text (t_client_info *info, char *text, 
                                  GtkWidget *box, void (*callback) 
                                  (GtkWidget *widget, t_client_info *info));
void mx_init_message_box (t_client_info *info, GtkWidget *box, 
                    void (*callback) (GtkWidget *widget, t_client_info *info));
void mx_init_menu (t_client_info *info);
void mx_profile_callback (GtkWidget *widget, t_client_info *info);
void mx_create_callback (GtkWidget *widget, t_client_info *info);
void mx_logout_callback (GtkWidget *widget, t_client_info *info);
void mx_delete_acc_callback(GtkWidget *widget, GtkWidget *answer_menu);
void mx_close_menu_callback (GtkWidget *widget, GdkEventButton *event, 
                             t_client_info *info);
void mx_yep_callback(GtkWidget *widget, t_client_info *info);
GtkWidget *mx_init_menu_main_box (t_client_info *info, GtkWidget *parent, 
                                    char *style);
GtkWidget *mx_init_menu_fixed (GtkWidget *main_box);
GtkWidget *mx_init_menu_box (GtkWidget *fixed, int size);
GtkWidget *mx_init_menu_exit_box (t_client_info *info, GtkWidget *parent,
                        void (*callback) (GtkWidget *widget, GdkEventButton *event, 
                        t_client_info *info));
void mx_create_room_callback (GtkWidget *widget, t_client_info *info);
void mx_close_creation_callback (GtkWidget *widget, GdkEventButton *event, 
                                 t_client_info *info);
void mx_close_creation_callback1 (GtkWidget *widget, t_client_info *info);
void mx_init_create (t_client_info *info);
void mx_init_stickers (t_client_info *info, GtkWidget *box);
void mx_init_search (t_client_info *info);
void mx_search_callback (GtkWidget *widget, t_client_info *info);
void mx_scroll_callback (GtkWidget *widget, t_all *data);
void mx_leave_callback (GtkWidget *widget, t_all *data);
void mx_room_menu_callback(GtkWidget *widget, GdkEventButton *event, GtkWidget *menu);
t_room *mx_find_room_position(t_room *rooms, int position);
void mx_init_room_data (t_client_info *info, t_room *room, json_object *room_data, 
                        t_all *data);
void mx_init_room_box (t_room *room);
void mx_init_room_header (t_room *room);
void mx_init_room_window (t_room *room);
void mx_init_room_messsage_box (t_room *room);
void mx_load_room_history (t_all *data);
void mx_init_room (t_client_info *info ,t_room *room, int position, 
                   json_object *room_data);
void mx_init_room_menu(t_room *room, t_all *data);

void mx_item_callback (GtkWidget *widget, t_stik *stik);
void mx_choose_sticker_callback(GtkWidget *widget, GtkWidget *menu);
void mx_url_callback (GtkWidget *widget, GdkEventButton *event, void *data);
void mx_logout_client(t_client_info *info);

void mx_show_etc (t_client_info *info, json_object *new_json, GtkWidget *profile);
void mx_show_global_settings (t_client_info *info, json_object *new_json, 
                              GtkWidget *profile);
void mx_show_title (const char *title, GtkWidget *box);
void mx_show_login (t_client_info *info, json_object *new_json, GtkWidget *profile);
void mx_show_email (t_client_info *info, int id, json_object *new_json, GtkWidget *profile);
void mx_show_id (t_client_info *info, int id, GtkWidget *profile);
void mx_show_name (t_client_info *info, int id, json_object *new_json, GtkWidget *profile);
void mx_show_exit (t_client_info *info,  GtkWidget *profile);
void mx_edit_name_callback (GtkWidget *widget, t_client_info *info);
void mx_save_name_callback (GtkWidget *widget, t_client_info *info);
void mx_edit_email_callback (GtkWidget *widget, t_client_info *info);
void mx_save_email_callback (GtkWidget *widget, t_client_info *info);
void mx_close_profile_callback1 (GtkWidget *widget, 
    GdkEventButton *event, t_client_info *info);
void mx_close_profile_callback (GtkWidget *widget, t_client_info *info);
void mx_show_empty_result (GtkWidget *room_box, char *title);
void mx_show_users_result (GtkWidget *room_box, int n_users, 
                        json_object *users, t_client_info *info);
void mx_show_rooms_result (GtkWidget *room_box, int n_rooms, 
                        json_object *rooms, t_client_info *info);
GtkWidget *mx_init_search_h_box (GtkWidget *room_box);
void mx_join_callback (GtkWidget *widget, t_all *data);
GtkWidget *mx_init_content_box (GtkWidget *v_box, char *title);
void mx_init_message_data (t_client_info *info, t_room *room, 
                            json_object *new_json, t_mes *mes);
void mx_init_message (t_message *node, t_room *room, 
                        json_object *new_json, int order);
void mx_init_main_message (t_message *node, t_mes *mes);
void mx_focus1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes);
void mx_focus_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes);
void mx_init_message_login (t_message *node, t_mes *mes, const char *login);
void mx_init_message_menu (t_message *node, t_mes *mes);
void mx_delete_callback (GtkWidget *widget, t_mes *mes);
void mx_edit_callback (GtkWidget *widget, t_mes *mes);
void mx_focus_menu_callback(GtkWidget *widget, 
                    GdkEventButton *event, GtkWidget *menu);
void mx_open_menu_callback(GtkWidget *widget, 
                    GdkEventButton *event, GtkWidget *menu);
void mx_load_profile_callback(GtkWidget *widget, t_mes *mes);
void mx_choose_side (t_client_info *info, t_message *node, 
                    t_room *room, int order);
void mx_simple_message (t_message *node, const char *message);
void mx_file (t_mes *mes, t_message *node, const char *message);
void mx_image (t_message *node);
void mx_sticker (t_message *node, const char *message);
int mx_notebook_prepend(t_note *note);
int mx_add(t_mes *mes);
int mx_reorder(t_mes *mes);
void mx_direct_message_client(t_client_info *info, json_object *new_json);
void mx_show_gif(char *gif, char *event, t_room *room, json_object *new_json);
void mx_leave_room_client(t_client_info *info, json_object *new_json);
int mx_notebook_detach(t_note *note);
void mx_edit_profile_client(t_client_info *info, json_object *new_json);
void mx_join_room_client(t_client_info *info, json_object *new_json);
void mx_input_authentification(t_client_info *info, json_object *new_json);
void mx_load_history_client(t_client_info *info, json_object *new_json);
void mx_input_message(t_client_info *info, json_object *new_json);
void mx_delete_message_client(t_client_info *info, json_object *new_json);
void mx_edit_message_client(t_client_info *info, json_object *new_json);
int mx_login (t_client_info *info);
void mx_append_message(t_client_info *info, t_room *room, 
                       json_object *new_json);
void mx_audio (t_mes *mes, t_message *node);

/*Functions*/
int mx_detect_file_extention(char *filename);
int tls_send(struct tls *tls_socket, const char *content, int size);
void mx_print_curr_time(void);
char *mx_curr_time(void);
void mx_print_tid(const char *s);
void mx_err_exit(const char *err_msg);  // вивести помилку
int mx_err_return2(const char *err_msg, const char *err_msg2);
int mx_err_return3(const char *err_msg, const char *err_msg2, int value);
int mx_err_return(const char *err_msg);
void mx_sha_hash_password(char *password);
void mx_get_input(char *buffer);
int mx_get_input2(char *buffer);
void mx_report_tls(struct tls * tls_ctx, char * host);
void mx_print_client_address(struct sockaddr_storage client_address, 
                             socklen_t client_len);
char *mx_date_to_char(void);
/*Json*/
int mx_js_o_o_add(struct json_object *obj, const char *key,
                  struct json_object *val);
struct json_object *mx_js_o_o_get(const struct json_object *obj,
                                  const char *key);
json_bool mx_js_o_o_get_ex(const struct json_object *obj, const char *key,
                           struct json_object **value);
const char *mx_js_g_str(json_object *jso);
int mx_js_g_str_len(const struct json_object *obj);
int32_t mx_js_g_int(const struct json_object *obj);
struct json_object *mx_js_n_int(int32_t i);
int mx_js_s_int(struct json_object *obj, int new_value);
struct json_object *mx_js_n_o();
struct json_object *mx_js_n_str(const char *s);
const char *mx_js_o_to_js_str(struct json_object *obj);
char *mx_strhash(const char *to_hash);
char *mx_encrypt(char *str);
char *mx_decrypt(char *str);
json_object *mx_create_basic_json_object(int type);
void mx_print_json_object(struct json_object *jobj, const char *msg);
char *mx_record_audio(t_client_info *info);
void *mx_send_mail(char *login, char *receiver, char *message);
int mx_mail_data_sending(struct tls *tls_c, t_mail *mail);
void mx_send_format(int socket, const char *text);
void mx_send_format_tls(struct tls *tls, char *arg1, char *arg2,
                        char *arg3);
void mx_init_struct_mail(t_mail *mail, char *receiver, char *message);
int mx_connect_to_server(const char *hostname, const char *port);
struct tls *mx_create_tls(void);
int mx_check_response(const char *response);
int mx_wait_on_response(int socket, struct tls *tls, int reply_code);

/*Audio*/
t_a_play *mx_init_struct_audio(void);
void mx_init_struct_audio2(t_mes *mes);
int mx_play_sound_file(t_mes *mes, char *start_time, char *duration_t);
int mx_record(PaStream *stream, t_audio *data, t_a_snippet *sample_block,
              t_client_info *info);
int mx_process_stream(PaStream *stream, t_audio *data,
                      t_a_snippet *sample_block, int *i);
int mx_play_loop(t_mes *mes, t_a_file *audio, SF_INFO s_info,
                 PaStream* stream);
int mx_play_loop_simple(t_a_file *audio, SF_INFO s_info,
                       PaStream* stream);
char *mx_get_file_name(t_mes *mes);
void mx_send_file_from_bot(t_client_info *info, char *file_name);
#endif

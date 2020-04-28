#include "uchat.h"

static void init_work_for_db(t_server_info **i) {
	(*i)->wdb = (t_work *)malloc(sizeof(t_work));
	(*i)->wdb->user_id = (int *)malloc(sizeof(int) * MX_MAX_USERS_IN_ROOM);
	(*i)->wdb->user_sock = (int *)malloc(sizeof(int) * MX_MAX_USERS_IN_ROOM);
	(*i)->wdb->i = 0;
}

static int get_user_id(void *wdb, int argc, char **argv, char **col_name) {
	t_work *wdb1 = *(t_work *)wdb;
	int j = 0;

    (void)col_name;
    if (argc == 0)
    	return 1;
    wdb1->user_id[(wdb1->i)] = atoi(argv[j]);
    printf("wdb1->array[(wdb1->i)] = %d\n", wdb1->user_id[(wdb1->i)]);
    wdb1->i++;
    return 0;
}

static int get_user_sock(void *wdb, int argc, char **argv, char **col_name) {
	t_work *wdb1 = (t_work *)wdb;
	int j = 0;

    (void)col_name;
    if (argc == 0)
    	return 1;
    wdb1->user_sock[(wdb1->i)] = atoi(argv[j]);
    wdb1->i++;
    return 0;
}

static void mx_get_user_id(t_server_info **i, int room) {
    char *command = malloc(1024);

    sprintf(command, "SELECT user_id FROM room_user WHERE room_id=%d", room);
    printf("%s\n", command);
    if (sqlite3_exec((*i)->db, command, get_user_id, &((*i)->wdb), 0) != SQLITE_OK) {
        printf("Kомната пуста\n");
        return ;
    }
    printf("(*i)->wdb: \n");
    for(int j = 0; j < 4; j++)
        printf("[%d] ", (*i)->wdb->user_id[j]);
    mx_strdel(&command);
}

void mx_get_user_sock(t_server_info **i, int *user_id, int len) {
    char *command1 = malloc(1024);

    for (int j = 0; j <= len; i++) {
        if (user_id[j] && user_id[j] != 0) {
            sprintf(command1, "SELECT user_sock FROM users WHERE user_id=%d", user_id[j]);
            printf("%s\n", command1);
            if (sqlite3_exec((*i)->db, command1, get_user_sock, (*i)->wdb, 0) != SQLITE_OK) {
                printf("Не открыло базу данных, ты проебался\n");
                return ;
            }
        }
        mx_strdel(&command1);
    }
    printf("(*i)->wdb: \n");
    for(int j = 0; j < 4; j++)
        printf("[%d] ", (((*i)->wdb)->user_sock)[j]);
}

int *mx_get_users_sock_in_room(t_server_info **i, int room) {
    int len = 0;
	init_work_for_db(i);
	//get user_id
    mx_get_user_id(i, room);
    //get user_sock
    len = ((*i)->wdb)->i;
    ((*i)->wdb)->i = 0;
    mx_get_user_sock(i, ((*i)->wdb)->user_id, len);
    
    return ((*i)->wdb)->user_sock;
}

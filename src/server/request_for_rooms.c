#include "uchat.h"

static void init_work_for_db(t_server_info **i) {
	(*i)->wdb = (t_work *)malloc(sizeof(t_work));
	(*i)->wdb->user_id = (int *)malloc(sizeof(int) * MX_MAX_USERS_IN_ROOM);
	(*i)->wdb->user_sock = (int *)malloc(sizeof(int) * MX_MAX_USERS_IN_ROOM);
	(*i)->wdb->i = 0;
}

static int get_user_sock(void *point, int argc, char **argv, char **col_name) {
	t_work *wdb1 = (t_work *)(point);

    (void)col_name;
    (void)wdb1;
    if (argc == 0)
    	return 1;
    wdb1->user_sock[(wdb1->i)] = atoi(argv[1]);
    wdb1->user_id[(wdb1->i)] = atoi(argv[2]);
    (wdb1->i)++;
    return 0;
}

static void get_users_sock(t_server_info **i, t_work *wdb, int room) {
    char *command = malloc(1024);

    sprintf(command, "SELECT room_id, socket, users.id FROM room_user, users\
        WHERE room_id=%d and room_user.user_id=users.id and users.socket!=0;",\
            room);
    sqlite3_exec((*i)->db, command, get_user_sock, wdb, 0);
    mx_strdel(&command);
}

int *mx_get_users_sock_in_room(t_server_info **i, int room) {
	init_work_for_db(i);
    get_users_sock(i, (*i)->wdb, room);
    return ((*i)->wdb)->user_sock;
}

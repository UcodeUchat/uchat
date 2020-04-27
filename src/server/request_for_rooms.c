#include "uchat.h"

static int get_user_id(void *wdb, int argc, char **argv, char **col_name) {
	t_work *wdb1 = (t_work *)wdb;
	(void)wdb1;
    (void)col_name;
    for (int j = 0; j < argc; j++)
    	printf("argv[0] = %s\n", argv[j]);
    if (argc > 0)
        return 1;
    return 0;
}

int *mx_get_users_sock_in_room(t_server_info **i, int room) {
	(*i)->wdb = (t_work *)malloc(sizeof(t_work));
	(*i)->wdb->array = (int *)malloc(sizeof(int) * MX_MAX_USERS_IN_ROOM);
	char *command = malloc(1024);
	char *error = malloc(1024);

    sprintf(command, "SELECT user_id FROM rooms WHERE room_id=%d", room);
    printf("%s\n", command);
    if (sqlite3_exec((*i)->db, command, get_user_id, (*i)->wdb, &error) != SQLITE_OK) {
        printf("Не открыло базу данных, ты проебался\n");
        return NULL;
    }
    printf("error = %s\n", error);
    mx_strdel(&command);
    return (*i)->wdb->array;
}

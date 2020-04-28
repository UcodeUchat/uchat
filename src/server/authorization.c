#include "uchat.h"

int mx_authorization(t_server_info *i, t_package *p) {
	int valid = mx_check_client(i, p);

	mx_memset(p->data, 0, sizeof(p->data));
	mx_memset(p->login, 0, sizeof(p->login));
	mx_memset(p->password, 0, sizeof(p->password));
	if (valid == 1) {
        p->type = MX_AUTH_TYPE_V;
		p->add_info = MX_AUTH_TYPE_V;
		tls_write(p->client_tls_sock, p, MX_PACKAGE_SIZE);
		//Vse kruto, chel in system
		fprintf(stderr, "Your answer = 1\n");
	}
	else {
        p->type = MX_AUTH_TYPE_NV;
		p->add_info = MX_AUTH_TYPE_NV;
		tls_write(p->client_tls_sock, p, MX_PACKAGE_SIZE);
		//Uvi, but go to dick :)
		fprintf(stderr, "Your answer = 0\n");
	}
    if (p->type == MX_AUTH_TYPE_V){
        int *array = mx_get_users_sock_in_room(&i, 0);
        (void)array;
    }
	return 1;
}

static int search_data(void *p, int argc, char **argv, char **col_name) {
    (void)p;
    (void)col_name;
    printf("argv[0] = %s\n", argv[0]);
    if (argc > 0)
        return 1;
    printf("Login is free!\n");
    return 0;
}

int mx_search_in_db(t_server_info *i, t_package *p, char *l, char *pa) {
    char *command = malloc(1024);
    (void)pa;
    sprintf(command, "SELECT login FROM users WHERE login='%s'", l);
    printf("%s\n", command);
    if (sqlite3_exec(i->db, command, search_data, p, 0) != SQLITE_OK) {
        printf("User with this login exist in our Uchat!\n");
        return -1;
    }
    mx_strdel(&command);
    return 1;
}

static int get_user_id(void *p, int argc, char **argv, char **col_name) {
    int *id = (int *)p;

    (void)col_name;
    if (argc > 1 || !argv[0])
        return 1;
    printf("argv[0] = %s\n", argv[0]);
    *id = atoi(argv[0]);
    printf("id = %d\n", *id);
    return 0;
}

int mx_add_to_db(t_server_info *i, t_package *p, char *l, char *pa){
    char *command = malloc(1024);
    char *command1 = malloc(1024);
    int user_id = -1;
    
    (void)p;
    sprintf(command, "insert into users (socket, login, password, access)\
                values (0,'%s', '%s', 1);\nselect user_id from users where\
                login='%s'", l, pa, l);
    printf("%s\n", command);
    if (sqlite3_exec(i->db, command, get_user_id, &user_id, 0) != SQLITE_OK) {
        printf("User with this login exist in our Uchat!\n");
        return -1;
    }
    printf("User has been added to the database!!\n");
    mx_strdel(&command);

    printf("user_id = %d\n", user_id);
    // add user in general room

    sprintf(command1, "insert into room_user (room_id, user_id)\
                values (0,%d);", user_id);
    printf("%s\n", command1);
    if (sqlite3_exec(i->db, command1, NULL, NULL, 0) != SQLITE_OK) {
        printf("User_id wasnt added!\n");
        return -1;
    }
    printf("User_id was added in rooms table\n");
    mx_strdel(&command1);
    return 1;
}

int mx_registration(t_server_info *i, t_package *p) {
	char *login = strdup(p->login);
	char *pass = strdup(p->password);

    if (mx_search_in_db(i, p, login, pass) == -1) {
    	printf("STOP!!!\n");
    	p->add_info = MX_REG_TYPE_NV;
    }
    else{
    	printf("USER REG!\n");
    	mx_add_to_db(i, p, login, pass);
    	p->add_info = MX_REG_TYPE_V;
    }
    tls_write(p->client_tls_sock, p, MX_PACKAGE_SIZE);
    return 1;
}

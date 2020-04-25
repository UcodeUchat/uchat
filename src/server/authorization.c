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

int mx_add_to_db(t_server_info *i, t_package *p, char *l, char *pa){
	char *command = malloc(1024);
    
    (void)p;
    sprintf(command, "insert into users (socket, login, password, access)\
				values (0,'%s', '%s', 1);", l, pa);
    printf("%s\n", command);
    if (sqlite3_exec(i->db, command, NULL, NULL, 0) != SQLITE_OK) {
        printf("User with this login exist in our Uchat!\n");
        return -1;
    }
    printf("User has been added to the database!!\n");
    mx_strdel(&command);
    return 1;
}

int mx_registration(t_server_info *i, t_package *p) {
	char *login = strdup(p->login);
	char *pass = strdup(p->password);


	//printf ("Enter your login: \n");
    //int l_size = mx_get_input2(login);

    //printf ("Enter your password: \n");
    //int p_size = mx_get_input2(pass);
    //p_size = l_size;
    //check data exist in database 
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
    //if data is not exist - add them in db
    return 1;
}

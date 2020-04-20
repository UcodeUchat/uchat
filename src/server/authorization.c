#include "uchat.h"

int mx_authorization(t_server_info *i, t_package *p) {
	int valid = mx_check_client(i, p);

	if (valid == 1){
		write(p->client_sock, "1\n", 2);
		//Vse kruto, chel in system
		fprintf(stderr, "Your answer = 1\n");
	}
	else {
		write(p->client_sock, "0\n", 2);
		//Uvi, but go to dick :)
		fprintf(stderr, "Your answer = 0\n");
	}
	return 1;
}

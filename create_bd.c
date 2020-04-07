#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sqlite3.h>

#define MX_PATH_TO_DB "./server_db.bin"
#define SIZE_RQ 4096

static int print_users(void *status, int argc, char **argv, char **in) {
	if (*(int *)status == 0) {
		for (int i = 0; i < 5; i++) {
			printf("%s", in[i]);
			if (i < 2 || i == 3)
				printf("\t");
			if (i == 2)
				printf("\t\t");
		}
		printf("\n");
		(*(int *)status)++;
	}
	for (int i = 0; argv[i]; i++) {
		printf("%s", argv[i]);
		if (strlen(argv[i]) <= 8 && i >=2)
			printf("\t\t");
		else 
			printf("\t");
	}
	printf("\n");
	return 0;
}

static int print_rooms(void *status, int argc, char **argv, char **in) {
	if (*(int *)status == 0) {
		for (int i = 0; i < 2; i++) {
			printf("%s ", in[i]);
		}
		printf("\n");
		(*(int *)status)++;
	}
	for (int i = 0; argv[i]; i++) {
		printf("%s ", argv[i]);
		if (strlen(argv[i]) <= 8)
			printf("\t");
	}
	printf("\n");
	return 0;
}

int main(int argc, char *argv[]){
	if (argc != 2) {
		printf("YOU NEED ENTER ./FILENAME BD_NAME");
		return 0;
	}
	else {
		int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
		int sql;
		sqlite3 *db;
		char req[SIZE_RQ];
		char *err = 0;

		close(fd);
		sql = sqlite3_open(MX_PATH_TO_DB, &db);
		if (sql != SQLITE_OK) {
			fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(1);
		}
		sprintf(req, "\
				create table users (\
					user_id integer PRIMERY KEY, socket integer,\
					login text, password text, access integer);\n\
				\
				create table rooms (\
					room_id integer PRIMERY KEY, user_id integer, \
					FOREIGN KEY (user_id) REFERENCES users (user_id));\n\
				\
				create table msg_history (message_id integer PRIMERY KEY,\
					user_id integer, room_id integer, message text,\
					addition_cont text, access integer,\
					FOREIGN KEY (user_id) REFERENCES users (user_id),\
					FOREIGN KEY (room_id) REFERENCES rooms (room_id));\n");
		sql = sqlite3_exec(db, req, 0, 0, &err);
		if (err)
			fprintf(stderr, "%s\n", err);
		sqlite3_free(err);

		//занесение данных в таблицу

		char req2[SIZE_RQ];
		sql = sqlite3_open(MX_PATH_TO_DB, &db);
		if (sql != SQLITE_OK) {
			fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(1);
		}
		sprintf(req2, "\
				insert into users (user_id, socket, login, password, access)\
				values (0, 0,'mmasniy', '123123', 1);\n\
				insert into users (user_id, socket, login, password, access)\
				values (1, 0,'vkmetyk', '123123', 1);\n\
				insert into users (user_id, socket, login, password, access)\
				values (2, 0,'snikolayen', '123123', 1);\n\
				insert into users (user_id, socket, login, password, access)\
				values (3, 0,'mlibovych', '123123', 1);\n\
				insert into users (user_id, socket, login, password, access)\
                values (4, 0,'neo', '1', 1);\
				insert into rooms (room_id, user_id)\
				values (0, 0);\n\
				insert into rooms (room_id, user_id)\
				values (0, 1);");
		sql = sqlite3_exec(db, req2, 0, 0, &err);
		if (err)
			fprintf(stderr, "%s\n", err);
		sqlite3_free(err);

		//вывод инфы в таблицах

		char req3[SIZE_RQ];
		int count = 0;
		sql = sqlite3_open(MX_PATH_TO_DB, &db);
		if (sql != SQLITE_OK) {
			fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(1);
		}
		sprintf(req3, "select * from users;");
		sql = sqlite3_exec(db, req3, print_users, &count, &err);
		if (err)
			fprintf(stderr, "%s\n", err);
		sqlite3_free(err);

		printf("\n\n");

		char req4[SIZE_RQ];
		int count1 = 0;
		sql = sqlite3_open(MX_PATH_TO_DB, &db);
		if (sql != SQLITE_OK) {
			fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(1);
		}
		sprintf(req4, "select * from rooms;");
		sql = sqlite3_exec(db, req4, print_rooms, &count1, &err);
		if (err)
			fprintf(stderr, "%s\n", err);
		sqlite3_free(err);
	}
	return 0;
}
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
		for (int i = 0; i < 3; i++) {
			if (i != 2)
				printf("%s \t", in[i]);
		}
		printf("\t%s", in[2]);
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

static int print_room_user(void *status, int argc, char **argv, char **in) {
	if (*(int *)status == 0) {
		for (int i = 0; i < 3; i++) {
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

static int print_msg(void *status, int argc, char **argv, char **in) {
		for (int i = 0; in[i] && i < 6; i++) {
			printf("%s ", in[i]);
		}
		printf("\n");
		(*(int *)status)++;
	for (int i = 0; argv[i]; i++) {
		printf("%s ", argv[i]);
		if (i == 3)
			printf("  ");
		else if (strlen(argv[i]) <= 8)
			printf("\t   ");
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
					id INTEGER PRIMARY KEY AUTOINCREMENT, socket integer,\
					login text, password text, access integer);\n\
				\
				create table rooms (\
					id integer PRIMERY KEY, name text, access integer);\n\
				\
				create table room_user (\
					id INTEGER PRIMARY KEY AUTOINCREMENT, room_id integer,\
					user_id integer,\
					FOREIGN KEY (user_id) REFERENCES users (id)\
					FOREIGN KEY (room_id) REFERENCES rooms (id));\n\
				\
				create table msg_history (id integer PRIMERY KEY,\
					user_id integer, room_id integer, message text,\
					addition_cont text, time data,\
					FOREIGN KEY (user_id) REFERENCES users (id),\
					FOREIGN KEY (room_id) REFERENCES rooms (id));\n");
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
				insert into users (socket, login, password, access)\
				values (10,'mmasniy', '123123', 1);\n\
				insert into users (socket, login, password, access)\
				values (11,'vkmetyk', '123123', 1);\n\
				insert into users (socket, login, password, access)\
				values (12,'snikolayen', '123123', 1);\n\
				insert into users (socket, login, password, access)\
				values (13,'mlibovych', '123123', 1);\n\
				insert into users (socket, login, password, access)\
                values (0,'neo', '1', 1);\n\
				insert into rooms (id, name, access)\
				values (0, 'General', 1);\n\
				insert into room_user(room_id, user_id)\
				values (0, 1);\n\
				insert into room_user (room_id, user_id)\
				values (0, 2);\n\
				insert into room_user (room_id, user_id)\
				values (0, 3);\n\
				insert into room_user (room_id, user_id)\
				values (0, 4);\n\
				insert into msg_history (id, user_id, room_id, message,\
				addition_cont)\
				values (0, 0, 0, 'hello', 'mes');");
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

		printf("\n\n");

		char req6[SIZE_RQ];
		int count3 = 0;
		sql = sqlite3_open(MX_PATH_TO_DB, &db);
		if (sql != SQLITE_OK) {
			fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(1);
		}
		sprintf(req6, "select * from room_user;");
		sql = sqlite3_exec(db, req6, print_room_user, &count3, &err);
		if (err)
			fprintf(stderr, "%s\n", err);
		sqlite3_free(err);

		printf("\n\n");

		char req5[SIZE_RQ];
		int count2 = 0;
		sql = sqlite3_open(MX_PATH_TO_DB, &db);
		if (sql != SQLITE_OK) {
			fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(1);
		}
		sprintf(req5, "select * from msg_history;");
		sql = sqlite3_exec(db, req5, print_msg, &count1, &err);
		if (err)
			fprintf(stderr, "%s\n", err);
		sqlite3_free(err);
	}
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sqlite3.h>

void mx_insert_data(sqlite3 *db, char *err, char req2[]) {
    int sql = sqlite3_open("./server_db.bin", &db);
    
    if (sql != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    req2[sprintf(req2, "insert into users (socket, login, password, access)\
                values (0,'neo', '1', 1);\
                insert into user_notifications (user_id, visual, audio, email)\
                values (1, 0, 0, 0);\
                insert into rooms (id, name, access)values (0, 'General', 1);\
                insert into room_user(room_id, user_id) values (0, 1);\
                insert into msg_history (id, user_id, room_id, message,\
                addition_cont)\
                values (0, 0, 0, 'hello', 'mes');")] = '\0';
    sql = sqlite3_exec(db, req2, 0, 0, &err);
    err != 0 ? fprintf(stderr, "%s\n", err) : 0;
    sqlite3_free(err);
    sqlite3_close(db);
}

void mx_create_users_and_rooms(sqlite3 *db, char *err, char req[]) {
    int sql = sqlite3_open("./server_db.bin", &db);
    
    if (sql != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    req[sprintf(req, "\
                create table if not exists users (\
                    id INTEGER PRIMARY KEY AUTOINCREMENT, socket integer,\
                    login text(128), password text(128),\
                    name text(128), email text(128), access integer);\
                \
                create table if not exists rooms (\
                    id INTEGER PRIMARY KEY AUTOINCREMENT, name text(128),\
                    access integer);\n")] = '\0';
    sql = sqlite3_exec(db, req, 0, 0, &err);
    err != 0 ? fprintf(stderr, "%s\n", err) : 0;
    sqlite3_free(err);
    sqlite3_close(db);
}

void mx_create_r_us_and_us_not(sqlite3 *db, char *err, char req[]) {
    int sql = sqlite3_open("./server_db.bin", &db);
    
    if (sql != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    req[sprintf(req, "create table if not exists room_user (\
                id INTEGER PRIMARY KEY AUTOINCREMENT, room_id integer,\
                user_id integer, role integer,\
                FOREIGN KEY (user_id) REFERENCES users (id)\
                FOREIGN KEY (room_id) REFERENCES rooms (id));\n\
                create table if not exists user_notifications (\
                id INTEGER PRIMARY KEY AUTOINCREMENT, user_id integer,\
                visual integer, audio integer, email integer,\
                FOREIGN KEY (user_id) REFERENCES users (id));")] = '\0';
    sql = sqlite3_exec(db, req, 0, 0, &err);
    err != 0 ? fprintf(stderr, "%s\n", err) : 0;
    sqlite3_free(err);
    sqlite3_close(db);
}

void mx_create_history_and_dir_room(sqlite3 *db, char *err, char req[]) {
    int sql = sqlite3_open("./server_db.bin", &db);
    
    if (sql != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    req[sprintf(req, "create table if not exists msg_history ( id INTEGER\
                PRIMARY KEY AUTOINCREMENT, user_id integer, room_id integer,\
                message VARCHAR(1024), addition_cont text,\
                time DATETIME DEFAULT CURRENT_TIMESTAMP, FOREIGN KEY (user_id)\
                REFERENCES users (id), FOREIGN KEY (room_id) REFERENCES rooms\
                (id)); create table if not exists direct_rooms ( id INTEGER\
                PRIMARY KEY AUTOINCREMENT, first_id integer,second_id integer,\
                FOREIGN KEY (first_id) REFERENCES users (id),\
                FOREIGN KEY (second_id) REFERENCES users (id));")] = '\0';
    sql = sqlite3_exec(db, req, 0, 0, &err);
    err != 0 ? fprintf(stderr, "%s\n", err) : 0;
    sqlite3_free(err);
    sqlite3_close(db);
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("YOU NEED ENTER ./FILENAME BD_NAME");
        return 0;
    }
    else {
        int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
        sqlite3 *db;
        char req[4096];
        char *err = 0;

        close(fd);
        mx_create_users_and_rooms(db, err, req);
        mx_create_r_us_and_us_not(db, err, req);
        mx_create_history_and_dir_room(db, err, req);
        mx_insert_data(db, err, req);
    }
    return 0;
}
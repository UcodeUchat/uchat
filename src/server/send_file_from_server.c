#include "uchat.h"

typedef struct  s_file_tmp {
    pthread_mutex_t *mutex;
    struct tls *tls;
    char *file_name;
    int size;
    int file_id;
}               t_file_tmp;

t_file_tmp *set_variables(t_socket_list *csl) {
    t_file_tmp *new = malloc(sizeof(t_file_tmp));

    new->tls = csl->tls_socket;
    new->mutex = &csl->mutex;
    new->file_id = json_object_get_int(json_object_object_get(csl->obj, "file_id"));
    return new;
}

int check_is_object_valid(json_object *obj) {
    if (json_object_object_get(obj, "room_id")
        && json_object_object_get(obj, "user_id")
        && json_object_object_get(obj, "file_id"))
        return MX_OK;
    else
        return 1;
}

int res(void *arg, int argc, char **argv, char **col_name) {
    (void)col_name;
    (void)argc;
    (void)arg;
    if (argv[0])
        return 0;
    return 1;
}

int get_result(void *arg, int argc, char **argv, char **col_name) {
    json_object *obj = (json_object *)arg;

    (void)col_name;
    (void)argc;
    if (argv[0]) {
        json_object_object_add(obj, "name", json_object_new_string(argv[3]));
        return MX_OK;
    }
    return 1;
}

char *check_file_in_db_and_user_access(t_server_info *info, json_object *obj) {
    int file_id = json_object_get_int(json_object_object_get(obj, "file_id"));
    char command[1024];

    command[sprintf(command, "select * from msg_history where id='%d'",
                    file_id)] = '\0';
    if (sqlite3_exec(info->db, command, get_result, obj, NULL) != SQLITE_OK) {
		return NULL;
    }
    command[sprintf(command, "select * from room_user where user_id='%d' & room_id='%d'", json_object_get_int(json_object_object_get(obj, "user_id")), json_object_get_int(json_object_object_get(obj, "room_id")))] = '\0';
    if (sqlite3_exec(info->db, command, res, NULL, NULL) != SQLITE_OK) {
		return NULL;
    }
    return strdup(json_object_get_string(json_object_object_get(obj, "name")));
}

void file_is_not_exist(t_file_tmp *vars) {
    const char *json_string;
    json_object *obj_for_sending = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);

    json_object_object_add(obj_for_sending, "file_id",
        json_object_new_int(vars->file_id));
    json_object_object_add(obj_for_sending, "piece", json_object_new_int(-1));
    json_string = json_object_to_json_string(obj_for_sending);
    mx_save_send(vars->mutex, vars->tls, json_string, strlen(json_string));
    json_object_put(obj_for_sending);
}

int is_file_exist(char *file_name, t_file_tmp *vars) {
    struct stat buffer;
    char *full_file_name = mx_strjoin(MX_SAVE_FOLDER_IN_SERVER, file_name);
    int exist = stat(full_file_name, &buffer);

    if (exist == MX_OK) {
        vars->file_name = full_file_name;
        vars->size = buffer.st_size;
        return MX_OK;
    }
    else {
        file_is_not_exist(vars);
        free(vars);
        mx_strdel(&full_file_name);
        return 1;
    }
}

void start_sending(FILE *file, t_file_tmp *vars) {
    json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
    json_object *data = json_object_new_string("");
    const char *json_string;
    int readed = 1;
    char buffer[2048];

    json_object_object_add(send_obj, "piece", json_object_new_int(1));
    json_object_object_add(send_obj, "file_size", json_object_new_int(vars->size));
    json_object_object_add(send_obj, "file_id", json_object_new_int(vars->file_id));

    json_string = json_object_to_json_string(send_obj);
    mx_save_send(vars->mutex, vars->tls, json_string, strlen(json_string));
    json_object_object_del(send_obj, "file_size");
    json_object_set_int(json_object_object_get(send_obj, "piece"), 2);
    json_object_object_add(send_obj, "data", data);

    while(readed > 0 && !feof(file)) {
        readed = fread(buffer, 1, sizeof(buffer), file);
        json_object_set_string_len(data, buffer, readed);
        feof(file) ? json_object_set_int(json_object_object_get(send_obj, "piece"), 3) : 0;
        json_string = json_object_to_json_string(send_obj);
        mx_save_send(vars->mutex, vars->tls, json_string, strlen(json_string));
    }
    json_object_put(send_obj);
}

void *send_file(void *arg) {
    t_file_tmp *vars = (t_file_tmp *)arg;
    FILE *file;
        
    if ((file = fopen(vars->file_name, "r")) != NULL) {
        start_sending(file, vars);
        fclose(file);
    }
    mx_strdel(&vars->file_name);
    free(vars);
    return NULL;
}

int mx_send_file_from_server(t_server_info *info, t_socket_list *csl) {
    (void)info;

    if (check_is_object_valid(csl->obj) == MX_OK) {
        char *file = check_file_in_db_and_user_access(info, csl->obj);

        if (file != NULL) {
            t_file_tmp *vars = set_variables(csl);

            if (is_file_exist(file, vars) == MX_OK) {
                
                pthread_t thread_input;
                pthread_attr_t attr;

                pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                if (pthread_create(&thread_input, &attr, send_file, vars) == 0)
                    return 0;
                mx_strdel(&vars->file_name);
                free(vars);
                fprintf(stderr, "pthread_create for file failed\n");
            }
        }
    }
    return 1;
}

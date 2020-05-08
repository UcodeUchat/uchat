#include "uchat.h"

int check_is_object_valid(json_object *obj) {
    if (json_object_object_get(obj, "room_id")
        && json_object_object_get(obj, "user_id")
        && json_object_object_get(obj, "msg_id"))
        return MX_OK;
    else
        return 1;
}

char *check_file_in_db_and_user_access(json_object *obj) {
    (void)obj;
    return strdup("1.pdf");
}

void file_is_not_exist(t_socket_list *csl) {
    const char *json_string;
    json_object *obj_for_sending = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);

    json_object_object_add(obj_for_sending, "msg_id",
        json_object_new_int(json_object_get_int(json_object_object_get(csl->obj, "msg_id"))));
    json_object_object_add(obj_for_sending, "piece", json_object_new_int(-1));
    json_string = json_object_to_json_string(obj_for_sending);
    tls_send(csl->tls_socket, json_string, strlen(json_string));
    json_object_put(obj_for_sending);
}

int is_file_exist(char *file_name, t_socket_list *csl) {
    struct stat buffer;
    int exist = stat(file_name, &buffer);

    if (exist == MX_OK) {
        char *full_file_name = mx_strjoin(MX_SAVE_FOLDER_IN_SERVER, file_name);
        json_object *file_name_obj = json_object_new_string(full_file_name);

        json_object_object_add(csl->obj, "file_name", file_name_obj);
        json_object_object_add(csl->obj, "file_size",
                               json_object_new_int(buffer.st_size));
        mx_strdel(&full_file_name);
        return MX_OK;
    }
    else {
        file_is_not_exist(csl);
        return 0;
    }
}

void start_sending(FILE *file, t_socket_list *csl) {
    json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
    json_object *data = json_object_new_null();
    const char *json_string;
    int readed = 1;
    char buffer[2048];

    json_object_object_add(send_obj, "piece", json_object_new_int(1));
    json_object_object_add(send_obj, "file_size",
        json_object_new_int(json_object_get_int(json_object_object_get(csl->obj, "file_size"))));
    // json_object_object_add(send_obj, "room_id", json_object_new_int(0));
    json_object_object_add(send_obj, "msg_id",
        json_object_new_int(json_object_get_int(json_object_object_get(csl->obj, "msg_id"))));

    json_string = json_object_to_json_string(send_obj);
    tls_send(csl->tls_socket, json_string, strlen(json_string));
    json_object_object_del(send_obj, "file_size");
    json_object_set_int(json_object_object_get(send_obj, "piece"), 2);
    json_object_object_add(send_obj, "data", data);

    while(readed > 0 && !feof(file)) {
        readed = fread(buffer, 1, sizeof(buffer), file);
        json_object_set_string_len(data, buffer, readed);
        feof(file) ? json_object_set_int(json_object_object_get(send_obj, "piece"), 3) : 0;
        json_string = json_object_to_json_string(send_obj);
        tls_send(csl->tls_socket, json_string, strlen(json_string));
    }
    json_object_put(send_obj);
}

void *send_file(void *arg) {
    t_socket_list *csl = (t_socket_list *)arg;
    
    if (json_object_get_string(json_object_object_get(csl->obj, "file_name"))) {
        FILE *file;
        
        if ((file = fopen(json_object_get_string(json_object_object_get(csl->obj, "file_name")), "r")) != NULL) {
            start_sending(file, csl);
            fclose(file);
        }
    }
    return NULL;
}

int mx_send_file_from_server(t_server_info *info, t_socket_list *csl) {
    (void)info;

    if (check_is_object_valid(csl->obj) == MX_OK) {
        char *file = check_file_in_db_and_user_access(csl->obj);

        if (file != NULL) {
            if (is_file_exist(file, csl) == MX_OK) {
                pthread_t thread_input;
                pthread_attr_t attr;

                pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                if (pthread_create(&thread_input, &attr, send_file, csl) == 0)
                    return 0;
                else
                    printf("pthread_create for file failed\n");
            }
        }
    }
    return 1;
}

#include "uchat.h"

bool pick_file_to_send(t_client_info *info, FILE **file, json_object **obj) {
    (void)info;
    printf("pick_file_to_send\n");

    char file_path[256] = {0};
    char file_name[256] = {0};
    scanf("%s", file_path);
    strcat(file_name, file_path);
    //
    if ((*file = fopen(file_path, "r")) != NULL) {
        struct stat file_stat;

        stat(file_path, &file_stat);
        if (file_stat.st_size > 0 && file_stat.st_size <= MX_MAX_FILE_SIZE) {
        // need to lock file editing
            *obj = mx_create_basic_json_object(MX_FILE_SEND_TYPE);
            json_object_object_add(*obj, "file_name", json_object_new_string(file_name));
            json_object_object_add(*obj, "file_size", json_object_new_int(file_stat.st_size));
            return 0;
        //
        }
    }
    return 1;
}

void mx_send_file_from_client(t_client_info *info) {
    FILE *file;
    json_object *send_obj;

    if (pick_file_to_send(info, &file, &send_obj) == 0) {
        printf("file picked\n");
        int readed = 1;
        const char *json_string;
        char buffer[1024];
        json_object *data;

        json_object_object_add(send_obj, "piece", json_object_new_int(1));
        json_object_object_add(send_obj, "user_id", json_object_new_int(info->id));
        json_object_object_add(send_obj, "room_id", json_object_new_int(0)); // need to know

        json_string = json_object_to_json_string(send_obj);

        tls_send(info->tls_client, json_string, strlen(json_string));

        json_object_object_del(send_obj, "file_name");
        json_object_object_del(send_obj, "file_size");
        json_object_set_int(json_object_object_get(send_obj, "piece"), 2);
        data = json_object_new_string("");
        json_object_object_add(send_obj, "data", data);
        while(readed > 0 && !feof(file)) {
            readed = fread(buffer, 1, sizeof(buffer), file);
            json_object_set_string_len(data, buffer, readed);
            feof(file) ? json_object_set_int(json_object_object_get(send_obj, "piece"), 3) : 0;
            json_string = json_object_to_json_string(send_obj);
            tls_send(info->tls_client, json_string, strlen(json_string));
        }
        json_object_put(send_obj);
        // unlock file
        fclose(file);
    }
    printf("exit from function\n");
}

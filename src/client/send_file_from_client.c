#include "uchat.h"

bool pick_file_to_send(t_client_info *info, FILE **file, json_object **obj) {
    (void)info;
    printf("pick_file_to_send\n");

    struct stat file_stat;
// tmp
    char file_path[256] = {0};
    char file_name[256] = {0};
    scanf("%s", file_path);
    strcat(file_name, file_path);
    if (stat(file_path, &file_stat) != MX_OK)
        return 1;
//
    if (file_stat.st_size > 0 && file_stat.st_size <= MX_MAX_FILE_SIZE) {
        if (!(file_stat.st_mode & S_IFDIR) && (file_stat.st_mode & S_IFREG)) {
            if ((*file = fopen(file_path, "r")) != NULL) {
                *obj = mx_create_basic_json_object(MX_FILE_SEND_TYPE);
                json_object_object_add(*obj, "file_name", json_object_new_string(file_name));
                json_object_object_add(*obj, "file_size", json_object_new_int(file_stat.st_size));
                return 0;
            }
            else
                fprintf(stderr, "Хм, під час відкривання файлу щось пішло не так\n");
        }
        else
            fprintf(stderr, "Йой, непідтримуваний тип файлу\n");
    }
    else
        fprintf(stderr, "Схоже, що твій файл надто великий\nМаксимальний розмір файлу %d\n", MX_MAX_FILE_SIZE);
    return 1;
}
// struct stat file_stat;
// stat(file_path, &file_stat);
// printf("S_IFMT:%d, S_IFSOCK:%d, S_IFLNK:%d, S_IFREG:%d, S_IFBLK:%d, S_IFDIR:%d, S_IFCHR:%d, S_IFIFO:%d\n",
// file_stat.st_mode & S_IFMT ? 1 : 0,
// file_stat.st_mode & S_IFSOCK ? 1 : 0,
// file_stat.st_mode & S_IFLNK ? 1 : 0,
// file_stat.st_mode & S_IFREG ? 1 : 0,
// file_stat.st_mode & S_IFBLK ? 1 : 0,
// file_stat.st_mode & S_IFDIR ? 1 : 0,
// file_stat.st_mode & S_IFCHR ? 1 : 0,
// file_stat.st_mode & S_IFIFO ? 1 : 0);
void mx_send_file_from_client(t_client_info *info) {
    FILE *file;
    json_object *send_obj;

    if (pick_file_to_send(info, &file, &send_obj) == 0) {
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
            if (feof(file)) {
                json_object_set_int(json_object_object_get(send_obj, "piece"), 3);
                json_object_object_add(send_obj, "login", json_object_new_string(info->login));
            }
            json_string = json_object_to_json_string(send_obj);
            tls_send(info->tls_client, json_string, strlen(json_string));
        }
        json_object_put(send_obj);
        // unlock file
        fclose(file);
    }
    printf("exit from function\n");
}

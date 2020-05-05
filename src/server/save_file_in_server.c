#include "uchat.h"

// void set_file_name(json_object *obj) {
//     time_t t = time(NULL);
//     struct tm tm = *localtime(&t);
//     const char *file_name;
//     char *new_name;

//     file_name = json_object_get_string(json_object_object_get(obj, "file_name"));
//     new_name = mx_strnew(strlen(file_name) + 60);

//     sprintf(new_name, "%d_%02d_%02d_%02d_%02d_%02d_%s",
//             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
//             tm.tm_hour, tm.tm_min, tm.tm_sec, file_name);
//     json_object_set_string(json_object_object_get(obj, "file_name"), new_name);
//     mx_strdel(&new_name);
// }

// t_file_list *mx_create_new_file_list(t_socket_list *csl, json_object *obj) {
//     t_file_list *new_file = (t_file_list *)malloc(sizeof(t_file_list));
//     char *full_filename = mx_strjoin(MX_SAVE_FOLDER_IN_SERVER,
//         json_object_get_string(json_object_object_get(obj, "file_name")));

//     mx_memset(new_file, 0, sizeof(t_file_list));
//     new_file->file = fopen(full_filename, "w+");
//     mx_strdel(&full_filename);
//     if (new_file->file != NULL) {
//         new_file->user_id = json_object_get_int(json_object_object_get(obj, "user_id"));
//     }
// }

// int mx_add_new_file_server(t_file_list **input_files, t_socket_list *csl) {
//     int file_size = json_object_get_int(json_object_object_get(csl->obj, "file_size"));

//     if (file_size > 0 && file_size < MX_MAX_FILE_SIZE) {
//         t_file_list *new_file = mx_create_new_file_list(csl, csl->obj);
        
    

//     }
//     else
//         fprintf(stderr, "Bad file size: %d\n", file_size);
// }

int mx_save_file_in_server(t_server_info *info, t_socket_list *csl) {
    (void)info;
    printf("mx_save_file_in_server\n");
    printf("%s\n", json_object_to_json_string(csl->obj));
    return 0;
}

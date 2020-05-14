#include "uchat.h"

// if (mx_add_file_to_list_in_client(&(msg->info->input_files), msg->message->data, msg->message->id) == 0) {

int start_downloading_file_in_client(t_client_info *info, json_object *obj) {
    if (mx_add_file_to_list_in_client(&(info->input_files),
        json_object_get_int(json_object_object_get(obj, "file_id")),
        (char *)json_object_get_string(json_object_object_get(obj, "file_name")),
        json_object_get_int(json_object_object_get(obj, "file_size"))) == 0) {
        return 0;
    }
    else
        fprintf(stderr, "Can't add new file\n");
    return 1;
}

int add_data_to_file_client(t_file_list *list, json_object *obj) {
    t_file_list *add_to = mx_find_file_in_list(list, json_object_get_int(json_object_object_get(obj, "file_id")));
    const char *data = json_object_get_string(json_object_object_get(obj, "data"));

    if (add_to) {
        fwrite(data, 1,json_object_get_string_len(json_object_object_get(obj, "data")), add_to->file);
        return 0;
    }
    return 1;
}

int final_file_input_client(t_file_list **list, json_object *obj) {
    t_file_list *add_to = mx_find_file_in_list(*list, json_object_get_int(json_object_object_get(obj, "file_id")));
    const char *data = json_object_get_string(json_object_object_get(obj, "data"));

    if (add_to) {
        int file_size;

        fwrite(data, 1, json_object_get_string_len(json_object_object_get(obj, "data")), add_to->file);
        file_size = ftell(add_to->file);
        fclose(add_to->file);
        
        if (file_size == add_to->file_size) {
            printf("FILE SIZE IS OK\n");
            // sygnal that file downloaded
            mx_pop_file_list_in_client(list, add_to->id);
            return 0;
        }
        else {
            fprintf(stderr, "FILE SIZE ISN'T CORRECT. %d|%d\n", file_size, add_to->file_size);
            remove(add_to->file_name);
        }
        mx_pop_file_list_in_client(list, add_to->id);
    }
    else
        mx_err_exit("Can't find\n");
    return 1;
}

int mx_save_file_in_client(t_client_info *info, json_object *obj) {
    int piece = json_object_get_int(json_object_object_get(obj, "piece"));

    if (piece == 1) {
        return start_downloading_file_in_client(info, obj);
    }
    else if (piece == 2) {
        return add_data_to_file_client(info->input_files, obj);
    }
    else if (piece == 3) {
        return final_file_input_client(&(info->input_files), obj);
    }
    else if (piece == -1) {
        fprintf(stderr, "file now is not exist. id:%d\n", json_object_get_int(json_object_object_get(obj, "file_id")));
    }
    return 0;
}

int is_file_exist(char *file_name) {
    char *full_path = mx_strjoin(MX_SAVE_FOLDER_IN_CLIENT, file_name);
    struct stat buffer;
    int exist = stat(full_path, &buffer);

    mx_strdel(&full_path);
    if (exist == MX_OK)
        return MX_OK;
    else
        return 1;
}

int mx_load_file(t_mes *msg) {
// need to know msg_id (file), user_id, room_id, file_name
    printf("msg->id = %d\n", msg->id);

    if (is_file_exist(msg->message->data) != MX_OK) {
        if (mx_find_file_in_list(msg->info->input_files, msg->message->id) == NULL) {
            json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
            const char *send_str;

            json_object_object_add(send_obj, "file_id", json_object_new_int(msg->message->id));
            json_object_object_add(send_obj, "user_id", json_object_new_int(msg->info->id));
            json_object_object_add(send_obj, "room_id", json_object_new_int(msg->room->id));
            send_str = json_object_to_json_string(send_obj);
            tls_send(msg->info->tls_client, send_str, strlen(send_str));
            json_object_put(send_obj);
            printf("Sended\n");
        }
        else
            fprintf(stderr, "File %s in progress...\n", msg->message->data);
    }
    else
        printf("Open file...\n");
    return 0;
}

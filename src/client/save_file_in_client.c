#include "uchat.h"

void start_downloading_file_in_client(t_file_list *list, json_object *obj) {
    t_file_list *add_to = mx_find_file_in_list(list, json_object_get_int(json_object_object_get(obj, "file_id")));

    if (add_to) {
        add_to->file_size = json_object_get_int(json_object_object_get(obj, "file_size"));
    }
    else
        fprintf(stderr, "Can't find\n");
}

void add_data_to_file_client(t_file_list *list, json_object *obj) {
    t_file_list *add_to = mx_find_file_in_list(list, json_object_get_int(json_object_object_get(obj, "file_id")));
    const char *data = json_object_get_string(json_object_object_get(obj, "data"));

    if (add_to) {
        fwrite(data, 1,json_object_get_string_len(json_object_object_get(obj, "data")), add_to->file);
    }
}

void final_file_input_client(t_file_list **list, json_object *obj) {
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
        }
        else {
            fprintf(stderr, "FILE SIZE ISN'T CORRECT. %d|%d\n", file_size, add_to->file_size);
            remove(add_to->file_name);
        }
        mx_pop_file_list_in_client(list, add_to->id);
    }
    else
        mx_err_exit("Can't find\n");
}

int mx_save_file_in_client(t_client_info *info, json_object *obj) {
    int piece = json_object_get_int(json_object_object_get(obj, "piece"));

    if (piece == 1) {
        start_downloading_file_in_client(info->input_files, obj);
    }
    else if (piece == 2) {
        add_data_to_file_client(info->input_files, obj);
    }
    else if (piece == 3) {
        final_file_input_client(&(info->input_files), obj);
    }
    else if (piece == -1) {
        fprintf(stderr, "file now is not exist. id:%d\n", json_object_get_int(json_object_object_get(obj, "file_id")));
    }
    return 0;
}

int mx_download_file_from_server(t_mes *msg) {
// need to know msg_id (file), user_id, room_id, file_name
    printf("msg->id = %d\n", msg->id);
    if (mx_add_file_to_list_in_client(&(msg->info->input_files), msg->message->data, msg->message->id) == 0) {
        json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
        const char *send_str;

        json_object_object_add(send_obj, "file_id", json_object_new_int(msg->message->id));
        json_object_object_add(send_obj, "user_id", json_object_new_int(msg->info->id));
        json_object_object_add(send_obj, "room_id", json_object_new_int(msg->room->id));
        send_str = json_object_to_json_string(send_obj);
        tls_send(msg->info->tls_client, send_str, strlen(send_str));
        json_object_put(send_obj);
        printf("Sended\n");
        return 0;
    }
    return 1;
}

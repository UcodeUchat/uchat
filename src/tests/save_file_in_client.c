#include "uchat.h"

// if (mx_add_file_to_list_in_client(&(msg->info->input_files), msg->message->data, msg->message->id) == 0) {

int start_downloading_file_in_client(t_client_info *info, json_object *obj) {
    if (mx_add_file_to_list_in_client(&(info->input_files),
        mx_js_g_int(mx_js_o_o_get(obj, "file_id")),
        (char *)mx_js_g_str(mx_js_o_o_get(obj, "file_name")),
        mx_js_g_int(mx_js_o_o_get(obj, "file_size"))) == 0) {
        return 0;
    }
    else
        fprintf(stderr, "Can't add new file\n");
    return 1;
}

int add_data_to_file_client(t_file_list *list, json_object *obj) {
    t_file_list *add_to = mx_find_file_in_list(list, mx_js_g_int(mx_js_o_o_get(obj, "file_id")));
    const char *data = mx_js_g_str(mx_js_o_o_get(obj, "data"));

    if (add_to) {
        fwrite(data, 1,mx_js_g_str_len(mx_js_o_o_get(obj, "data")), add_to->file);
        return 0;
    }
    return 1;
}

int show_image(GtkWidget *image) {
    gtk_widget_show(image);
    return 0;
}

int final_file_input_client(t_file_list **list, json_object *obj, t_client_info *info) {
    t_file_list *add_to = mx_find_file_in_list(*list, mx_js_g_int(mx_js_o_o_get(obj, "file_id")));
    const char *data = mx_js_g_str(mx_js_o_o_get(obj, "data"));

    if (add_to) {
        int file_size;

        fwrite(data, 1, mx_js_g_str_len(mx_js_o_o_get(obj, "data")), add_to->file);
        file_size = ftell(add_to->file);
        fclose(add_to->file);
        
        if (file_size == add_to->file_size) {
            printf("FILE SIZE IS OK\n");
            mx_pop_file_list_in_client(list, add_to->id);
            if (mx_js_g_int(mx_js_o_o_get(obj, "add_info")) == 2) {
                const char *file_name = mx_js_g_str(mx_js_o_o_get(obj, "file_name"));
                char *extention = strdup(file_name);

                while (mx_get_char_index(extention, '.') >= 0) {
                    char *tmp = strdup(extention + mx_get_char_index(extention, '.') + 1);
                    free(extention);
                    extention = strdup(tmp);
                    free(tmp); 
                }

                char *file_path = mx_strjoin("./Uchat_downloads/", file_name);
                int file_id = mx_js_g_int(mx_js_o_o_get(obj, "file_id"));
                int room_id = mx_js_g_int(mx_js_o_o_get(obj, "room_id"));
                t_room *room = mx_find_room(info->data->rooms, room_id);
                t_message *message = mx_find_message(room->messages, file_id);
                GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale (file_path, 350, 290, TRUE, NULL);
                GtkWidget *image = NULL;
                if (strcmp(extention, "gif") == 0)
                    image = gtk_image_new_from_file(file_path);
                else
                    image = gtk_image_new_from_pixbuf(pixbuf);
                gtk_box_pack_start (GTK_BOX (message->image_box), image, FALSE, FALSE, 0);
                g_idle_add ((GSourceFunc)show_image, image);
                //g_idle_add ((GSourceFunc)show_image, message->h_box);
            }
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
    int piece = mx_js_g_int(mx_js_o_o_get(obj, "piece"));

    if (piece == 1) {
        return start_downloading_file_in_client(info, obj);
    }
    else if (piece == 2) {
        return add_data_to_file_client(info->input_files, obj);
    }
    else if (piece == 3) {
        return final_file_input_client(&(info->input_files), obj, info);
    }
    else if (piece == -1) {
        fprintf(stderr, "file now is not exist. id:%d\n", mx_js_g_int(mx_js_o_o_get(obj, "file_id")));
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

    if (msg->message->add_info == 2) {
        json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
        const char *send_str;

        mx_js_o_o_add(send_obj, "file_id", mx_js_n_int(msg->id));
        mx_js_o_o_add(send_obj, "user_id", mx_js_n_int(msg->info->id));
        mx_js_o_o_add(send_obj, "room_id", mx_js_n_int(msg->room->id));
        send_str = mx_js_o_to_js_str(send_obj);
        tls_send(msg->info->tls_client, send_str, strlen(send_str));
        json_object_put(send_obj);
        printf("Sended\n");
    }
    else if (is_file_exist(msg->message->data) != MX_OK ) {
        if (mx_find_file_in_list(msg->info->input_files, msg->message->id) == NULL) {
            json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
            const char *send_str;

            mx_js_o_o_add(send_obj, "file_id", mx_js_n_int(msg->id));
            mx_js_o_o_add(send_obj, "user_id", mx_js_n_int(msg->info->id));
            mx_js_o_o_add(send_obj, "room_id", mx_js_n_int(msg->room->id));
            send_str = mx_js_o_to_js_str(send_obj);
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

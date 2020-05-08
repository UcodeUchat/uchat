#include "uchat.h"

int mx_save_file_in_client(t_client_info *info, json_object *obj) {
    (void)info;
    (void)obj;

    // if (package->piece == 1) {
    //     printf("1=%d\n", mx_add_new_file_client(&(info->input_files), package) ? 1 : 0);
    // }
    // else if (package->piece == 2 || package->piece == 3) {
    //     printf("2=%d\n", mx_add_data_to_file_client(&(info->input_files), package) ? 1 : 0);
    //     if (package->piece == 3)
    //         printf("3=%d\n", mx_final_file_input_client(info, package) ? 1 : 0);
    // }
    return 0;
}

int mx_download_file_from_server(t_client_info *info) {
    json_object *send_obj = mx_create_basic_json_object(MX_FILE_DOWNLOAD_TYPE);
    // char *full_file_name = mx_strjoin(MX_SAVE_FOLDER_IN_CLIENT, "1.pdf");
    const char *send_str;

// need to know msg_id (file), user_id, room_id, file_name
    json_object_object_add(send_obj, "msg_id", json_object_new_int(10)); //
    json_object_object_add(send_obj, "user_id", json_object_new_int(info->id));
    json_object_object_add(send_obj, "room_id", json_object_new_int(0)); //
//
    send_str = json_object_to_json_string(send_obj);
    tls_send(info->tls_client, send_str, strlen(send_str));
    json_object_put(send_obj);
    return 0;
}

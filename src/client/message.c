#include "uchat.h"

static void choose_type (t_mes *mes, t_message *node, const char *message) {
    mes->info->cant_play = 1;
    if (node->add_info == 0)
        mx_simple_message (node, message);
    else if (node->add_info == 1) 
        mx_file (mes, node, message);
    else if (node->add_info == 2)
        mx_image (node);
    else if (node->add_info == 3)
        mx_sticker (node, message);
    else if (node->add_info == 4)
        mx_audio (mes, node);
}

t_message *mx_create_message (t_client_info *info, t_room *room, 
                json_object *new_json, int order) {
    t_message *node =  (t_message *)malloc(sizeof(t_message));
    t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
    const char *login = mx_js_g_str(mx_js_o_o_get(new_json, "login"));
    const char *message = mx_js_g_str(mx_js_o_o_get(new_json, "data"));

    mx_init_message(node, room, new_json, order);
    mx_init_message_data(info, room, new_json, mes);
    mx_init_main_message(node, mes);
    mx_init_message_menu(node, mes);
    mx_init_message_login(node, mes, login);
    mes->message = node;
    choose_type(mes, node, message);
    mx_choose_side (info, node, room, order);
    mes->audio = mx_init_struct_audio();
    node->next = NULL;
    return node;
}

static void *sound_thread (void *data) {
    (void)data;

    mx_play_audio_notific("./sounds/message_receive.aif", "0", "1");
    return 0;
}

static void additional_act (t_client_info *info, t_room *room, 
                            json_object *new_json, t_message *tmp) {
    int id = mx_js_g_int(mx_js_o_o_get(new_json, "id"));
    int add_info = mx_js_g_int(mx_js_o_o_get(new_json, "add_info"));
    int user_id = mx_js_g_int(mx_js_o_o_get(new_json, "user_id"));

    if (info->audio == 1) {
        if (user_id != info->id) {
            pthread_t sound_t = NULL;
            pthread_create(&sound_t, 0, sound_thread, NULL);
        }
    }
    if (add_info == 2) {
        t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
        mes->info = info;
        mes->room = room;
        mes->id = id;
        mes->message = tmp;
        mx_load_file(mes);
    }
}

void mx_push_message(t_client_info *info, t_room *room, json_object *new_json) {
    t_message *tmp;
    t_message *p;
    t_message **list = &room->messages;

    if (!list)
        return;
    tmp = mx_create_message(info, room, new_json, 1);  // Create new
    if (!tmp)
        return;
    p = *list;
    if (*list == NULL) {  // Find Null-node
        *list = tmp;
    }
    else {
        while (p->next != NULL)  // Find Null-node
            p = p->next;
        p->next = tmp;
    }
    additional_act (info, room, new_json, tmp);
}

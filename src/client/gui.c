#include "uchat.h"

t_room *find_room(t_room *rooms, int position) {
   t_room *head = rooms;
   t_room *node = NULL;

    while (head != NULL) {
        if (head->position == position) {
            node = head;
            break;
        }
        head = head->next;
    }
    return node;
}

void mx_choose_file_callback(GtkWidget *widget, t_client_info *info) {
    (void)widget;
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = find_room(info->data->rooms, position);
    info->data->current_room = room->id;
    mx_send_file_from_client(info, NULL);
}

void choose_sticker_callback(GtkWidget *widget, GtkWidget *menu) {
    (void)widget;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

void sleep_ms (int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void *msg_history_thread (void *data) {
    t_all *data1 = (t_all *)data;
    struct json_object *messages;
    json_object_object_get_ex(data1->room_data, "messages", &messages);
    int n_msg = json_object_array_length(messages);
    for (int j = 0; j < n_msg; j++) {
        json_object *msg_data = json_object_array_get_idx(messages, j);
        append_message(data1->info, data1->room, msg_data);
    }
    gtk_adjustment_set_value(data1->room->Adjust, 
                            gtk_adjustment_get_upper(data1->room->Adjust) - 
                            gtk_adjustment_get_page_size(data1->room->Adjust) + 2.0);
    return 0;
}

void *login_msg_thread (void *data) {
    t_client_info *n_data = (t_client_info *)data;

    sleep(3);
    //n_data->data->login_msg_flag = 0;
    gtk_widget_hide(n_data->data->login_msg);
    gtk_widget_hide(n_data->data->stop);
    return 0;
}

void *register_msg_thread (void *data) {
    t_client_info *n_data = (t_client_info *)data;

    sleep(3);
    //n_data->data->login_msg_flag = 0;
    gtk_widget_hide(n_data->data->register_msg);
    //gtk_widget_hide(n_data->data->stop);
    return 0;
}

void mx_send_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    if (strcmp(gtk_entry_get_text(GTK_ENTRY(info->data->message_entry)),"") != 0) {
        if (info->editing < 0) {
            int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
            t_room *room = find_room(info->data->rooms, position);
            info->data->current_room = room->id;
            mx_process_message_in_client(info);
        }
        else {
            json_object *new_json;
            char *message = strdup(gtk_entry_get_text(GTK_ENTRY(info->data->message_entry)));

            new_json = json_object_new_object();
            json_object_object_add(new_json, "type", json_object_new_int(MX_EDIT_MESSAGE_TYPE));
            json_object_object_add(new_json, "login", json_object_new_string(info->login));
            json_object_object_add(new_json, "data", json_object_new_string (message));
            json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
            json_object_object_add(new_json, "room_id", json_object_new_int(info->editing_room));
            json_object_object_add(new_json, "message_id", json_object_new_int(info->editing));
            json_object_object_add(new_json, "add_info", json_object_new_int(0));
            //mx_print_json_object(new_json, "edit");
            const char *json_string = json_object_to_json_string(new_json);
            tls_send(info->tls_client, json_string, strlen(json_string));
            gtk_widget_hide(info->data->edit_button);
            info->editing = -1;
        }
        gtk_entry_set_text(GTK_ENTRY(info->data->message_entry), "");
    }
}

void mx_record_callback (GtkWidget *widget, t_client_info *info) {
    char *audio_file = mx_record_audio();
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = find_room(info->data->rooms, position);
    
    info->data->current_room = room->id;
    printf("new %s\n", audio_file);
    mx_send_file_from_client(info, audio_file);
    (void)widget;
}

int validation (char *login, char *password, char *repeat_password) {
    if (!login || strlen(login) < 6) {
        printf("Твой логин хуйня\n");
        return 0;
    }
    if (!password || strlen(password) < 6) {
        printf("Твой пароль хуйня\n");
        return 0;
    }
    if (strcmp(password, repeat_password) != 0) {
        printf("Пароли не совпадают\n");
        return 0;
    }
    return 1;
}

void mx_send_data_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    char *login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->login_entry));
    char *password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->password_entry));
    char *repeat = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->repeat_password_entry));
    if (validation(login, password, repeat)) {
        json_object *json_obj = mx_create_basic_json_object(MX_REG_TYPE);
        const char *json_str;

        json_object_object_add(json_obj, "login", json_object_new_string(login));
        json_object_object_add(json_obj, "password", json_object_new_string(password));

        mx_print_json_object(json_obj, "login send_data_callback");
        json_str = json_object_to_json_string(json_obj);

        tls_send(info->tls_client, json_str, strlen(json_str));

        gtk_widget_hide(info->data->register_box);
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
        gtk_widget_show(info->data->login_box);
        //succes thread
    }
    else {
        pthread_cancel(info->data->register_msg_t);
        gtk_widget_show(info->data->register_msg);
        // gtk_widget_show(info->data->stop);
        pthread_create(&info->data->register_msg_t, 0, register_msg_thread, info);
    }
}

void mx_cancel_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Sign in");
    gtk_widget_hide(info->data->register_box);
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
    gtk_widget_show(info->data->login_box);
}

void url_callback (GtkWidget *widget, GdkEventButton *event, void *data) {
    (void)widget;
    (void)event;
    (void)data;
    system("open https://ucode.world");
}

void mx_init_main_title(t_client_info *info, GtkWidget *screen)  {
    GtkWidget *table = gtk_grid_new();
    GtkWidget *title1 = gtk_label_new("Ucode");
    GtkWidget *title2 = gtk_label_new("chat");
    GtkWidget *box = gtk_box_new(FALSE, 0);
    GtkWidget *event_box = gtk_event_box_new();

    gtk_widget_set_name(title1, "title1");
    gtk_grid_attach (GTK_GRID (table), title1, 0, 0, 1, 1);
    gtk_widget_set_name(title2, "title2");
    gtk_grid_attach (GTK_GRID (table), title2, 1, 0, 1, 1);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (screen), box, 0, 125);
    gtk_container_add (GTK_CONTAINER (event_box), table);
    gtk_widget_add_events (event_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (event_box), "button_press_event", 
        G_CALLBACK (url_callback), NULL);
    gtk_box_pack_start (GTK_BOX (box), event_box, TRUE, FALSE, 0);
    gtk_widget_show_all(box);
}

void mx_reg_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Registration");
    gtk_widget_hide(info->data->login_box);
    gtk_widget_show (info->data->register_box);
}

void logout(t_client_info *info) {
    json_object *new_json;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_LOGOUT_TYPE));
    //json_object_object_add(new_json, "login", json_object_new_string(info->login));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    mx_print_json_object(new_json, "logout");
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}

void mx_delete_account(t_client_info *info) {
    json_object *new_json;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_DELETE_ACCOUNT_TYPE));
    json_object_object_add(new_json, "login", json_object_new_string(info->login));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    mx_print_json_object(new_json, "delete_account");
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
}

void mx_yep_callback(GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->auth_client = 0;
    mx_delete_account(info);
    logout(info);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Login");
    gtk_entry_set_text(GTK_ENTRY(info->data->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->password_entry), "");
    gtk_widget_destroy(info->data->general_box);
    gtk_widget_show (info->data->login_box);
}

void mx_close_creation_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_hide(info->data->create_room->main_box);
}

void mx_close_creation_callback1 (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_hide(info->data->create_room->main_box);
}

void mx_create_room_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    if (strcmp ("", gtk_entry_get_text(GTK_ENTRY(info->data->create_room->name_entry))) != 0) {
        json_object *new_json;

        gtk_widget_hide(info->data->create_room->main_box);
        new_json = json_object_new_object();
        json_object_object_add(new_json, "type", json_object_new_int(MX_CREATE_ROOM_TYPE));
        json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
        json_object_object_add(new_json, "name", json_object_new_string 
                                (gtk_entry_get_text(GTK_ENTRY(info->data->create_room->name_entry))));
        json_object_object_add(new_json, "acces", json_object_new_int 
                                (gtk_combo_box_get_active (GTK_COMBO_BOX(info->data->create_room->selection)) + 1));
        json_object  *room_data = json_object_new_object();
        json_object *messages = json_object_new_array();
        json_object_object_add(room_data, "messages", messages);
        json_object_object_add(room_data, "name", json_object_new_string 
                                (gtk_entry_get_text(GTK_ENTRY(info->data->create_room->name_entry))));
        json_object_object_add(new_json, "room_data", room_data);
        const char *json_string = json_object_to_json_string(new_json);
        tls_send(info->tls_client, json_string, strlen(json_string));
        gtk_entry_set_text(GTK_ENTRY(info->data->create_room->name_entry), "");
        gtk_widget_hide(info->data->menu);
    }
}

void mx_delete_acc_callback(GtkWidget *widget, GtkWidget *answer_menu) {
    (void)widget;
    gtk_menu_popup_at_pointer (GTK_MENU(answer_menu), NULL);
}

void scroll_callback (GtkWidget *widget, t_all *data) {
    (void)widget;
    if (gtk_adjustment_get_value(data->room->Adjust) == 
        gtk_adjustment_get_lower(data->room->Adjust) && data->info->can_load == 1 && data->room->messages != NULL) {
        json_object  *new_json = json_object_new_object();

        data->info->can_load = 0;
        json_object_object_add(new_json, "type", json_object_new_int(MX_LOAD_MORE_TYPE));
        json_object_object_add(new_json, "room_id", json_object_new_int(data->room->id));
        json_object_object_add(new_json, "last_id", json_object_new_int(data->room->messages->id));
        mx_print_json_object(new_json, "load 15 more");
        const char *json_str = json_object_to_json_string(new_json);
        tls_send(data->info->tls_client, json_str, strlen(json_str));
    }
}


void leave_callback (GtkWidget *widget, t_all *data) {
    (void)widget;
    json_object  *new_json = json_object_new_object();

    json_object_object_add(new_json, "type", json_object_new_int(MX_LEAVE_ROOM_TYPE));
    json_object_object_add(new_json, "room_id", json_object_new_int(data->room->id));
    json_object_object_add(new_json, "access", json_object_new_int(data->room->access));
    json_object_object_add(new_json, "user_id", json_object_new_int(data->info->id));
    json_object_object_add(new_json, "login", json_object_new_string(data->info->login));
    const char *json_str = json_object_to_json_string(new_json);
    tls_send(data->info->tls_client, json_str, strlen(json_str));
}

void room_menu_callback(GtkWidget *widget, GdkEventButton *event, GtkWidget *menu) {
    (void)widget;
    (void)event;
    gtk_menu_popup_at_pointer (GTK_MENU(menu), NULL);
}

void mx_logout_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->auth_client = 0;
    logout(info);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Login");
    gtk_entry_set_text(GTK_ENTRY(info->data->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->password_entry), "");
    gtk_widget_destroy(info->data->general_box);
    gtk_widget_show (info->data->login_box);
}

void mx_close_menu_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_hide(info->data->menu);
}

void mx_menu_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show(info->data->menu);

}

void mx_edit_cancel_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    info->editing = -1;
    gtk_entry_set_text(GTK_ENTRY(info->data->message_entry), "");
    gtk_widget_hide(info->data->edit_button);
}

void mx_load_profile_client(t_client_info *info, int id) {
    json_object *new_json;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_LOAD_PROFILE_TYPE));
    json_object_object_add(new_json, "id", json_object_new_int(id));
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
    gtk_widget_hide(info->data->menu);
}

void mx_profile_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    mx_load_profile_client(info, info->id);
}

void mx_create_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show(info->data->create_room->main_box);
}

void mx_search_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    json_object *new_json;

    gtk_widget_hide(info->data->search_box);
    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_SEARCH_ALL_TYPE));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    json_object_object_add(new_json, "query", json_object_new_string 
                            (gtk_entry_get_text(GTK_ENTRY(info->data->search_entry))));
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
    gtk_entry_set_text(GTK_ENTRY(info->data->search_entry), "");
}

void mx_show_search_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show_all(info->data->search_box);
}

void item_callback (GtkWidget *widget, t_stik *stik) {
    (void)widget;
    json_object *new_json;

    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(stik->info->data->notebook));
    t_room *room = find_room(stik->info->data->rooms, position);
    stik->info->data->current_room = room->id;
    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_MSG_TYPE));
    json_object_object_add(new_json, "login", json_object_new_string(stik->info->login));
    json_object_object_add(new_json, "data", json_object_new_string(stik->name));
    json_object_object_add(new_json, "user_id", json_object_new_int(stik->info->id));
    json_object_object_add(new_json, "room_id", json_object_new_int(stik->info->data->current_room));
    json_object_object_add(new_json, "add_info", json_object_new_int(3));
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(stik->info->tls_client, json_string, strlen(json_string));
}

char *get_extention (char *path) {
    char *extention = strdup(path);
    char *tmp = NULL;

    while (mx_get_char_index(extention, '.') >= 0) {
        tmp = strdup(extention + mx_get_char_index(extention, '.') + 1);
        free(extention);
        extention = strdup(tmp);
        free(tmp); 
    }
    return extention;
}

void init_sticker_item (t_client_info *info, char *path, struct dirent  *ds1, GtkWidget *sub_menu) {
    GtkWidget *item = gtk_menu_item_new();
    char *name = mx_strjoin(path, "/");
    char *tmp = mx_strjoin(name, ds1->d_name);
    GdkPixbuf *item_pixbuf = NULL;
    GtkWidget *item_image = NULL;

    free(name);
    name = strdup(tmp);
    free(tmp);
    item_pixbuf = gdk_pixbuf_new_from_file_at_scale (name, 80, 80, TRUE, NULL);
    item_image = gtk_image_new_from_pixbuf(item_pixbuf);
    gtk_container_add (GTK_CONTAINER (item), item_image);
    gtk_widget_show(item_image);
    gtk_widget_show(item);
    gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), item);
    t_stik *stik = (t_stik *)(malloc)(sizeof(t_stik));
    stik->info = info;
    stik->name = strdup(name);
    g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (item_callback), stik);
}

void init_sticker_pack (t_client_info *info, char *path, struct dirent  *ds, GtkWidget *menu) {
    DIR *sub_dir = opendir(path);
    char *extention = NULL;
    GtkWidget *sub_menu = NULL;
    struct dirent  *ds1;
    GtkWidget *sub_item;

    if (sub_dir != NULL) {
        sub_menu  = gtk_menu_new ();
        while ((ds1 = readdir(sub_dir)) != 0) {
            extention = get_extention (ds1->d_name);
            if(strcmp(extention, "png") == 0)
                init_sticker_item (info, path, ds1, sub_menu);
        }
        sub_item = gtk_menu_item_new_with_label(ds->d_name);
        gtk_widget_show(sub_item);
        gtk_menu_item_set_submenu (GTK_MENU_ITEM (sub_item), sub_menu);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), sub_item);
        closedir(sub_dir);
    }
}

void mx_init_stickers (t_client_info *info, GtkWidget *box) {
    GtkWidget *menu  = gtk_menu_new ();
    DIR *dptr  = opendir("stickers");
    char *path = NULL;
    struct dirent *ds;

    if (dptr != NULL) {
        while ((ds = readdir(dptr)) != 0) {
            path = mx_strjoin("stickers/", ds->d_name);
            if (ds->d_name[0] != '.') {
                init_sticker_pack (info, path, ds, menu);
            }
        }
        closedir(dptr);
    }

    GtkWidget *button = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/smile.png", 20, 20, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(button), image);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(choose_sticker_callback), menu);
    gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 0);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
}

int mx_notebook_prepend(t_note *note) {
    gtk_notebook_append_page(GTK_NOTEBOOK(note->notebook), note->box, note->label);
    gtk_notebook_reorder_child(GTK_NOTEBOOK(note->notebook), note->box, note->position);
    gtk_notebook_set_current_page (GTK_NOTEBOOK(note->notebook), note->position);
    return 0;
}

void load_room_history (t_all *data) {
    struct json_object *messages;
    json_object_object_get_ex(data->room_data, "messages", &messages);
    int n_msg = json_object_array_length(messages);
    for (int j = 0; j < n_msg; j++) {
        json_object *msg_data = json_object_array_get_idx(messages, j);
        append_message(data->info, data->room, msg_data);
    }
    gtk_adjustment_set_value(data->room->Adjust, 
                            gtk_adjustment_get_upper(data->room->Adjust) - 
                            gtk_adjustment_get_page_size(data->room->Adjust) + 2.0);
}

void init_room (t_client_info *info ,t_room *room, int position, json_object *room_data) {
    char *name = strdup(json_object_get_string(json_object_object_get(room_data, "name")));
    int id = json_object_get_int(json_object_object_get(room_data, "room_id"));
    int access = json_object_get_int(json_object_object_get(room_data, "access"));

    if (access == 3)
        room->name = mx_replace_substr (name, info->login, "");
    else
        room->name = strdup(name);
    room->position = position;
    room->id = id;
    room->access = access;
    room->messages = NULL;
    room->next = NULL;
    free(name);
}

void init_room_menu(t_room *room, t_all *data) {
    GtkWidget *history = NULL;
    GtkWidget *leave = NULL;

    room->room_menu  = gtk_menu_new ();
    history = gtk_menu_item_new_with_label ("Load history");
    gtk_widget_show(history);
    gtk_menu_shell_append (GTK_MENU_SHELL (room->room_menu), history);
    g_signal_connect (G_OBJECT (history), "activate", G_CALLBACK (scroll_callback), data);
    
    if (room->id != 0 && room->access != 3) {
        leave = gtk_menu_item_new_with_label ("Leave room");
        gtk_widget_show(leave);
        gtk_menu_shell_append (GTK_MENU_SHELL (room->room_menu), leave);
        g_signal_connect (G_OBJECT (leave), "activate", G_CALLBACK (leave_callback), data);
    }
}

void init_room_data (t_client_info *info, t_room *room, json_object *room_data, t_all *data) {
    data->info = info;
    data->room = room;
    data->room_data = room_data;
}

void init_room_box (t_room *room) {
    room->room_box = gtk_box_new(FALSE, 0);
    gtk_widget_set_name(room->room_box, "mesage_box");
    gtk_orientable_set_orientation (GTK_ORIENTABLE(room->room_box), GTK_ORIENTATION_VERTICAL);
    gtk_widget_show(room->room_box);
}

void init_room_header (t_room *room) {
    room->header = gtk_event_box_new();
    gtk_widget_set_size_request(room->header, -1, 40);
    gtk_widget_add_events (room->header, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (room->header), "button_press_event", G_CALLBACK (room_menu_callback), room->room_menu);
    GtkWidget *full_name = gtk_label_new(room->name);
    gtk_widget_set_name (full_name, "title");
    gtk_container_add (GTK_CONTAINER (room->header), full_name);
    gtk_widget_show(full_name);
    gtk_box_pack_start (GTK_BOX (room->room_box), room->header, FALSE, FALSE, 0);
    gtk_widget_show(room->header);
}

void init_room_window (t_room *room) {
    GtkWidget *ptrVscrollBar = NULL;

    room->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_box_pack_start (GTK_BOX (room->room_box), room->scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show(room->scrolled_window);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(room->scrolled_window),
                                GTK_POLICY_NEVER,
                                GTK_POLICY_AUTOMATIC);
    room->Adjust = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(room->scrolled_window));
    ptrVscrollBar = gtk_scrolled_window_get_vscrollbar(GTK_SCROLLED_WINDOW(room->scrolled_window));
    gtk_widget_set_name (ptrVscrollBar, "bar");
}

void init_room_messsage_box (t_room *room) {
    room->message_box = gtk_box_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(room->message_box), 5);
    gtk_container_add(GTK_CONTAINER(room->scrolled_window), room->message_box);
    gtk_widget_show(room->message_box);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(room->message_box), GTK_ORIENTATION_VERTICAL);
}

char *trim_name (t_room *room) {
    char *str = NULL;
    char *tmp = NULL;

    if (strlen(room->name) > 15) {
        str = strndup(room->name, 12);
        tmp = mx_strjoin(str, "...");
        free (str);
        str = strdup (tmp);
        free(tmp);
    }
    else 
        str = strdup(room->name);
    return str;
}

void show_room (t_client_info *info, t_room *room, int position) {
    char *str = trim_name (room);
    GtkWidget *label = gtk_label_new(str);
    t_note *note = (t_note *)malloc(sizeof(t_note));

    note->notebook = info->data->notebook;
    note->box = room->room_box;
    note->label = label;
    note->position = position;
    g_idle_add ((GSourceFunc)mx_notebook_prepend, note);
}

t_room *mx_create_room (t_client_info *info, json_object *room_data, int position) {
    t_room *room =  (t_room *)malloc(sizeof(t_room));
    t_all *data = (t_all *)malloc(sizeof(t_all));

    init_room (info, room, position, room_data);
    init_room_data (info, room ,room_data, data);
    init_room_box (room);
    init_room_menu (room, data);
    init_room_header (room);
    init_room_window (room);
    init_room_messsage_box (room);
    load_room_history (data);
    show_room (info, room, position);
    return room;
}

void mx_push_room(t_client_info *info, json_object *room_data, int position) {
    t_room *tmp;
    t_room *p;
    t_room **list = &info->data->rooms;

    if (!list)
        return;
    tmp = mx_create_room(info, room_data, position);  // Create new
    if (!tmp)
        return;
    p = *list;
    if (*list == NULL) {  // Find Null-node
        *list = tmp;
        return;
    }
    else {
        while (p->next != NULL)  // Find Null-node
            p = p->next;
        p->next = tmp;
    }
}

void authentification(t_client_info *info) {
    json_object *js;
    const char *json_string = NULL;

    js = json_object_new_object();
    json_object_object_add(js, "type", json_object_new_int(MX_AUTH_TYPE));
    json_object_object_add(js, "login", json_object_new_string(info->login));
    json_object_object_add(js, "password", json_object_new_string(info->password));
    mx_print_json_object(js, "login authentification");
    json_string = json_object_to_json_string(js);
    tls_send(info->tls_client, json_string, strlen(json_string));
    while (info->responce == 0) {
        (void)info;
    }
    info->responce = 0;
}

void mx_enter_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->login_entry));
    info->password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->password_entry));
    authentification(info);
    if (info->auth_client == 0) {
        pthread_cancel(info->data->login_msg_t);
        gtk_widget_show(info->data->login_msg);
        gtk_widget_show(info->data->stop);
        pthread_create(&info->data->login_msg_t, 0, login_msg_thread, info);
    }
    else if(info->auth_client == 1) {
        mx_init_general (info);
        mx_init_menu (info);
        mx_init_search (info);
        mx_init_create (info);
    }  
}

void init_window(t_client_info *info) {
    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path (provider,"my_style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                               GTK_STYLE_PROVIDER(provider),
                               GTK_STYLE_PROVIDER_PRIORITY_USER);
    info->data = (t_data *)malloc(sizeof(t_data));
    info->data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(info->data->window, "back"); 
    gtk_window_set_resizable (GTK_WINDOW (info->data->window), FALSE);
    gtk_widget_set_size_request (GTK_WIDGET (info->data->window), 1000, 630);
    gtk_widget_show (info->data->window);
    gtk_window_set_title (GTK_WINDOW (info->data->window), "Sign in");
    g_signal_connect (G_OBJECT (info->data->window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect_swapped (G_OBJECT (info->data->window), "delete_event",
                              G_CALLBACK (gtk_widget_destroy), 
                              G_OBJECT (info->data->window));
    info->data->main_box = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (info->data->window), info->data->main_box);
    gtk_widget_show (info->data->main_box);
}

int mx_login (t_client_info *info) {
    init_window(info);
    mx_init_reg(info);
    mx_init_login(info);

    gtk_main();
    return 0;
}

#include "uchat.h"

typedef struct s_all {
    t_client_info *info;
    t_room *room;
    struct json_object *room_data;
}               t_all;

typedef struct s_stik {
    t_client_info *info;
    char *name;
}               t_stik;

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

void choose_file_callback(GtkWidget *widget, t_client_info *info) {
    (void)widget;
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = find_room(info->data->rooms, position);
    info->data->current_room = room->id;
    mx_send_file_from_client(info);
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
        //sleep_ms(10);
    }
    gtk_adjustment_set_value(data1->room->Adjust, 
                            gtk_adjustment_get_upper(data1->room->Adjust) - 
                            gtk_adjustment_get_page_size(data1->room->Adjust) + 2.0);
    return 0;
}

void *login_msg_thread (void *data) {
    t_client_info *n_data = (t_client_info *)data;

        if (n_data->data->login_msg_flag) {
            sleep(3);
            if (n_data->data->login_msg_flag) {
                n_data->data->login_msg_flag = 0;
                gtk_widget_hide(n_data->data->login_msg);
                gtk_widget_hide(n_data->data->stop);
            }
        }
    return 0;
}


t_room *create_room(void *name, int id, int position) {
    t_room *node =  (t_room *)malloc(sizeof(t_room));

    node->name = strdup(name);
    node->position = position;
    node->id = id;
    node->messages = NULL;
    node->next = NULL;
    return node;
}

void push_room(t_room **list, void *name, int id, int position) {
    t_room *tmp;
    t_room *p;

    if (!list)
        return;
    tmp = create_room(name, id, position);  // Create new
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

void send_callback (GtkWidget *widget, t_client_info *info) {
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

void record_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    (void)info;

    char *audio_file = mx_record_audio();
    printf("new racord %s\n", audio_file);

//    mx_send_file_from_client(info);

    /*
    json_object *new_json;
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
    gtk_entry_set_text(GTK_ENTRY(info->data->message_entry), "");
     */
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

void send_data_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    char *login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->login_entry));
    char *password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->password_entry));
    char *repeat = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->repeat_password_entry));
    if (validation(login, password, repeat)) {
        t_package *p = mx_create_new_package();
        strncat(p->login, login, sizeof(p->login) - 1);
        strncat(p->password, password, sizeof(p->password) - 1);
        p->type = MX_REG_TYPE;


        json_object  *new_json = mx_package_to_json(p);
        mx_print_json_object(new_json, "login send_data_callback");
        const char *json_str = json_object_to_json_string(new_json);

        tls_send(info->tls_client, json_str, strlen(json_str));

        gtk_widget_hide(info->data->register_box);
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
        gtk_widget_show(info->data->login_box);
        //succes thread
    }
    else {
        printf("Не брат ты мне, гнида не завалидированная\n");
    }
}

void cancel_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Sign in");
    gtk_widget_hide(info->data->register_box);
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
    gtk_widget_show(info->data->login_box);
}

void init_reg(t_client_info *info) {
    info->data->register_box = gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->register_box, 0, 0);
    

    info->data->registration = (t_reg *)malloc(sizeof(t_reg));
    info->data->registration->login_entry = gtk_entry_new ();
    //--stop image
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/uk1.png", 400, 320, TRUE, NULL);
    info->data->registration->stop = gtk_image_new_from_pixbuf(pixbuf);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), info->data->registration->stop, 200, 150);
    gtk_widget_show(info->data->registration->stop);
    //--
    gtk_entry_set_max_length (GTK_ENTRY (info->data->registration->login_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->registration->login_entry), "Write your login(6+ chars)");
    gtk_editable_select_region (GTK_EDITABLE (info->data->registration->login_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->registration->login_entry)));
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 200);
    gtk_box_pack_start (GTK_BOX (box), info->data->registration->login_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->registration->login_entry);
    gtk_widget_set_name(info->data->registration->login_entry, "entry");

    info->data->registration->password_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->registration->password_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->registration->password_entry), "Write your password(6+ chars)");
    gtk_editable_select_region (GTK_EDITABLE (info->data->registration->password_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->registration->password_entry)));
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 250);
    gtk_box_pack_start (GTK_BOX (box), info->data->registration->password_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->registration->password_entry);
    gtk_widget_set_name(info->data->registration->password_entry, "entry");

    info->data->registration->repeat_password_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->registration->repeat_password_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->registration->repeat_password_entry), "Repeat your password");
    gtk_editable_select_region (GTK_EDITABLE (info->data->registration->repeat_password_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->registration->repeat_password_entry)));
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 300);
    gtk_box_pack_start (GTK_BOX (box), info->data->registration->repeat_password_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->registration->repeat_password_entry);
    gtk_widget_set_name(info->data->registration->repeat_password_entry, "entry");
                                                    
    GtkWidget *button = gtk_button_new_with_label("Register");
    g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (send_data_callback),info);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 350);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");

    button = gtk_button_new_with_label("Cancel");
    g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (cancel_callback),info);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 390);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");
}


void reg_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Registration");
    gtk_widget_hide(info->data->login_box);
    gtk_widget_show (info->data->register_box);
}

void logout(t_client_info *info) {
    json_object *new_json;

    new_json = json_object_new_object();
    json_object_object_add(new_json, "type", json_object_new_int(MX_LOGOUT_TYPE));
    json_object_object_add(new_json, "login", json_object_new_string(info->login));
    json_object_object_add(new_json, "user_id", json_object_new_int(info->id));
    mx_print_json_object(new_json, "logout");
    const char *json_string = json_object_to_json_string(new_json);
    tls_send(info->tls_client, json_string, strlen(json_string));
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

void logout_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->auth_client = 0;
    logout(info);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Login");
    gtk_entry_set_text(GTK_ENTRY(info->data->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->password_entry), "");
    gtk_widget_hide(info->data->general_box);
    gtk_widget_show (info->data->login_box);
}

void close_menu_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_hide(info->data->menu);
}

void menu_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show(info->data->menu);

}

void edit_cancel_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
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

void profile_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    mx_load_profile_client(info, info->id);
}

void init_menu (t_client_info *info) {
    info->data->menu = gtk_box_new(FALSE, 0);
    gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->menu, 0, 0);
    gtk_widget_set_size_request(info->data->menu, gtk_widget_get_allocated_width (info->data->window), 
                                gtk_widget_get_allocated_height (info->data->window));
    GtkWidget *main_box = gtk_event_box_new();
    gtk_box_pack_start (GTK_BOX (info->data->menu), main_box, FALSE, FALSE, 0);
    gtk_widget_set_size_request(main_box, 150, -1);
    gtk_widget_set_name (main_box, "menu_main");
    
    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(main_box), fixed);
    GtkWidget *box = gtk_box_new(FALSE, 10);
    gtk_widget_set_size_request(box, 150, -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    gtk_fixed_put (GTK_FIXED (fixed), box, 0, 10);
    //--buttons
    GtkWidget *box1 = gtk_box_new(FALSE, 0);
    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    GtkWidget *button = gtk_button_new_with_label("Create room");
    gtk_box_pack_start (GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
    gtk_widget_show(box1);
    box1 = gtk_box_new(FALSE, 0);
    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    button = gtk_button_new_with_label("Profile");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(profile_callback), info);
    gtk_box_pack_start (GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
    gtk_widget_show(box1);
    box1 = gtk_box_new(FALSE, 0);
    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    button = gtk_button_new_with_label("Logout");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(logout_callback), info);
    gtk_box_pack_start (GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
    gtk_widget_show(box1);
    box1 = gtk_box_new(FALSE, 0);
    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    button = gtk_button_new_with_label("Blacklist");
    gtk_box_pack_start (GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
    gtk_widget_show(box1);
    //--
    gtk_widget_show (box);
    gtk_widget_show(fixed);
    gtk_widget_show(main_box);
    
    GtkWidget *exit_box = gtk_event_box_new();
    gtk_box_pack_start (GTK_BOX (info->data->menu), exit_box, TRUE, TRUE, 0);
    gtk_widget_realize (exit_box);
    gtk_widget_add_events (exit_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (exit_box), "button_press_event", G_CALLBACK (close_menu_callback), info);
    gtk_widget_set_name (exit_box, "menu_exit");
    gtk_widget_show(exit_box);
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

void init_stickers (t_client_info *info, GtkWidget *box) {
    GtkWidget *menu  = gtk_menu_new ();
    DIR *dptr  = opendir("stickers");

    if (dptr != NULL) {
        struct dirent  *ds;

        while((ds = readdir(dptr)) != 0) {//cчитываем хуйню из директории
            char *path = mx_strjoin("stickers/", ds->d_name);

            if (ds->d_name[0] != '.') {
                DIR *sub_dir = opendir(path);

                if (sub_dir != NULL) {
                    GtkWidget *sub_menu  = gtk_menu_new ();
                    struct dirent  *ds1;

                    while((ds1 = readdir(sub_dir)) != 0) {//cчитываем хуйню из директории
                        char *extention = strdup(ds1->d_name);

                        while (mx_get_char_index(extention, '.') >= 0) {
                            char *tmp = strdup(extention + mx_get_char_index(extention, '.') + 1);
                            free(extention);
                            extention = strdup(tmp);
                            free(tmp); 
                        }
                        if(strcmp(extention, "png") == 0) {
                            GtkWidget *item = gtk_menu_item_new();
                            char *name = mx_strjoin(path, "/");
                            char *tmp = mx_strjoin(name, ds1->d_name);
                            free(name);
                            name = strdup(tmp);
                            free(tmp);
                            GdkPixbuf *item_pixbuf = gdk_pixbuf_new_from_file_at_scale (name, 80, 80, TRUE, NULL);
                            GtkWidget *item_image = gtk_image_new_from_pixbuf(item_pixbuf);
                            gtk_container_add (GTK_CONTAINER (item), item_image);
                            gtk_widget_show(item_image);
                            gtk_widget_show(item);
                            gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), item);
                            t_stik *stik = (t_stik *)(malloc)(sizeof(t_stik));
                            stik->info = info;
                            stik->name = strdup(name);
                            g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (item_callback), stik);
                        }
                    }
                    GtkWidget *sub_item = gtk_menu_item_new_with_label(ds->d_name);
                    gtk_widget_show(sub_item);
                    gtk_menu_item_set_submenu (GTK_MENU_ITEM (sub_item), sub_menu);
                    gtk_menu_shell_append (GTK_MENU_SHELL (menu), sub_item);
                    closedir(sub_dir);
                }
            }
        }
        closedir(dptr);
    }
    //--
    GtkWidget *s_button = gtk_button_new();
    GdkPixbuf *pixbuf2 = gdk_pixbuf_new_from_file_at_scale ("img/smile.png", 20, 20, TRUE, NULL);
    GtkWidget *image2 = gtk_image_new_from_pixbuf(pixbuf2);
    gtk_button_set_image(GTK_BUTTON(s_button), image2);
    g_signal_connect(G_OBJECT(s_button), "clicked", G_CALLBACK(choose_sticker_callback), menu);
    gtk_box_pack_start (GTK_BOX (box), s_button, FALSE, FALSE, 0);
    gtk_widget_set_name(s_button, "entry");
    gtk_widget_show(s_button);
}

void init_general (t_client_info *info) {
    info->data->profile = NULL;
    info->can_load = 1;
    info->data->rooms = NULL;
    info->data->current_room = 0;
    info->data->general_box = gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->general_box, 0, 0);
    //--
    GtkWidget *box = gtk_box_new(FALSE, 10);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window) - 20, -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->general_box), box, 10, 570);
    gtk_widget_show (box);
    //--Menu button
    info->data->menu_button = gtk_button_new();
    GtkWidget *image0 = gtk_image_new_from_file("img/a.png");
    gtk_button_set_image(GTK_BUTTON(info->data->menu_button), image0);
    g_signal_connect(G_OBJECT(info->data->menu_button), "clicked", G_CALLBACK(menu_callback), info);
    gtk_box_pack_start (GTK_BOX (box), info->data->menu_button, FALSE, FALSE, 0);
    gtk_widget_set_name(info->data->menu_button, "entry");
    gtk_widget_show(info->data->menu_button);
    //--message
    GtkWidget *fixed_message = gtk_fixed_new();
    info->data->message_entry = gtk_entry_new ();
    gtk_entry_set_placeholder_text(GTK_ENTRY (info->data->message_entry), "Write something");
    gtk_entry_set_max_length(GTK_ENTRY (info->data->message_entry), 100);
    gtk_editable_select_region(GTK_EDITABLE (info->data->message_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->message_entry)));
    g_signal_connect(G_OBJECT(info->data->message_entry),"activate", G_CALLBACK(send_callback), info);
    gtk_box_pack_start (GTK_BOX (box), fixed_message, TRUE, TRUE, 0);
    gtk_fixed_put(GTK_FIXED(fixed_message), info->data->message_entry, 0, 0);
    gtk_widget_set_size_request(info->data->message_entry, 600, -1);
    gtk_widget_set_size_request(fixed_message, 600, -1);
    gtk_widget_set_name(info->data->message_entry, "entry");
    gtk_widget_show(info->data->message_entry);
    gtk_widget_show(fixed_message);
    //--Edit button
    info->data->edit_button = gtk_event_box_new ();
    gtk_widget_add_events (info->data->edit_button, GDK_BUTTON_PRESS_MASK);
    GdkPixbuf *pixbuf0 = gdk_pixbuf_new_from_file_at_scale ("img/cancel.png", 20, 20, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf0);
    gtk_container_add (GTK_CONTAINER (info->data->edit_button), image);
    gtk_widget_show(image);
    g_signal_connect(G_OBJECT(info->data->edit_button), "button_press_event", G_CALLBACK(edit_cancel_callback), info);
    gtk_fixed_put(GTK_FIXED(fixed_message), info->data->edit_button, 570, 7);

    //--Send button
    info->data->send_button = gtk_button_new_with_label("Send");
    g_signal_connect(G_OBJECT(info->data->send_button), "clicked", G_CALLBACK(send_callback), info);
    gtk_box_pack_start (GTK_BOX (box), info->data->send_button, FALSE, FALSE, 0);
    gtk_widget_set_size_request(info->data->send_button, 75, -1);
    gtk_widget_set_name(info->data->send_button, "entry");
    gtk_widget_show(info->data->send_button);

    //--record button
    info->data->record_button = gtk_button_new_with_label("Record");
    g_signal_connect(G_OBJECT(info->data->record_button), "clicked", G_CALLBACK(record_callback), info);
    gtk_box_pack_start (GTK_BOX (box), info->data->record_button, FALSE, FALSE, 0);
    gtk_widget_set_size_request(info->data->record_button, 75, -1);
    gtk_widget_set_name(info->data->record_button, "entry");
    gtk_widget_show(info->data->record_button);

    //--File selection
    info->data->file_button = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/file.png", 20, 20, TRUE, NULL);
    GtkWidget *image1 = gtk_image_new_from_pixbuf(pixbuf);
    gtk_button_set_image(GTK_BUTTON(info->data->file_button), image1);
    g_signal_connect(G_OBJECT(info->data->file_button), "clicked", G_CALLBACK(choose_file_callback), info);
    gtk_box_pack_start (GTK_BOX (box), info->data->file_button, FALSE, FALSE, 0);
    gtk_widget_set_name(info->data->file_button, "entry");
    gtk_widget_show(info->data->file_button);
    //--sticker selection
    init_stickers(info, box);
    //--notebook
    info->data->notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK (info->data->notebook), GTK_POS_LEFT);
    gtk_widget_set_size_request(info->data->notebook, gtk_widget_get_allocated_width (info->data->window) -  20, 550);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->general_box), box, 0, 10);
    gtk_box_pack_start (GTK_BOX (box), info->data->notebook, TRUE, FALSE, 0);
    gtk_widget_show (info->data->notebook);
    gtk_widget_show (box);

    int n_rooms = json_object_array_length(info->rooms);
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(info->rooms, i);
        char *str = strdup(json_object_get_string(json_object_object_get(room_data, "name")));
        int id = json_object_get_int(json_object_object_get(room_data, "room_id"));

        push_room(&info->data->rooms, str, id, i);
        t_room *room = find_room(info->data->rooms, i);
        //--
        t_all *data = (t_all *)malloc(sizeof(t_all));
        data->info = info;
        data->room = room;
        data->room_data = room_data;
        //--
        room->room_box = gtk_box_new(FALSE, 0);
        gtk_widget_set_name(room->room_box, "mesage_box");
        gtk_orientable_set_orientation (GTK_ORIENTABLE(room->room_box), GTK_ORIENTATION_VERTICAL);
        gtk_widget_show(room->room_box);
        //--
        //--room menu
        GtkWidget *room_menu  = gtk_menu_new ();
        //--items
        if (id != 0) {
            GtkWidget *leave = gtk_menu_item_new_with_label("Leave room");
            gtk_widget_show(leave);
            gtk_menu_shell_append (GTK_MENU_SHELL (room_menu), leave);
            g_signal_connect (G_OBJECT (leave), "activate", G_CALLBACK (leave_callback), data);
        }

        GtkWidget *history = gtk_menu_item_new_with_label("Load history");
        gtk_widget_show(history);
        gtk_menu_shell_append (GTK_MENU_SHELL (room_menu), history);
        g_signal_connect (G_OBJECT (history), "activate", G_CALLBACK (scroll_callback), data);
        //--
        GtkWidget *event = gtk_event_box_new();
        gtk_widget_set_size_request(event, -1, 40);
        gtk_widget_add_events (event, GDK_BUTTON_PRESS_MASK);
        g_signal_connect (G_OBJECT (event), "button_press_event", G_CALLBACK (room_menu_callback), room_menu);
        GtkWidget *full_name = gtk_label_new(str);
        gtk_widget_set_name (full_name, "title");
        gtk_container_add (GTK_CONTAINER (event), full_name);
        gtk_widget_show(event);
        gtk_box_pack_start (GTK_BOX (room->room_box), event, FALSE, FALSE, 0);
        gtk_widget_show(full_name);

        room->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
        gtk_box_pack_start (GTK_BOX (room->room_box), room->scrolled_window, TRUE, TRUE, 0);
        // gtk_container_set_border_width(GTK_CONTAINER(room->scrolled_window), 1);
        gtk_widget_show(room->scrolled_window);

        room->Adjust = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(room->scrolled_window));
        GtkWidget *ptrVscrollBar = gtk_scrolled_window_get_vscrollbar(GTK_SCROLLED_WINDOW(room->scrolled_window));
        gtk_widget_set_name (ptrVscrollBar, "bar");

        if (strlen(str) > 15) {
            str = strndup(str, 12);
            str = mx_strjoin(str, "...");
        }
        GtkWidget *label = gtk_label_new(str);
        gtk_notebook_append_page(GTK_NOTEBOOK(info->data->notebook), room->room_box, label);
        //--
        room->message_box = gtk_box_new(FALSE, 5);
        gtk_container_set_border_width(GTK_CONTAINER(room->message_box), 5);
        gtk_container_add(GTK_CONTAINER(room->scrolled_window), room->message_box);
        gtk_widget_show(room->message_box);
        gtk_orientable_set_orientation (GTK_ORIENTABLE(room->message_box), GTK_ORIENTATION_VERTICAL);
        //--
        //--msg history
        pthread_t msg_history_t = NULL;
        pthread_create(&msg_history_t, 0, msg_history_thread, data);
        //--
    }
    gtk_widget_hide(info->data->login_box);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Uchat");
    gtk_widget_show(info->data->general_box);
}

void authentification(t_client_info *info) {
    json_object *js;

    js = json_object_new_object();
    json_object_object_add(js, "type", json_object_new_int(MX_AUTH_TYPE));
    json_object_object_add(js, "login", json_object_new_string(info->login));
    json_object_object_add(js, "password", json_object_new_string(info->password));
    mx_print_json_object(js, "login authentification");
    const char *json_string = json_object_to_json_string(js);
    tls_send(info->tls_client, json_string, strlen(json_string));
    while (info->responce == 0) {

    }
    info->responce = 0;
}

void close_widget_callback (GtkWidget *widget, GtkWidget *widget_ptr) {
    (void)widget;
    gtk_widget_hide(widget_ptr);
}

void enter_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    pthread_t login_msg_t = NULL;
    //--auth
    info->login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->login_entry));
    info->password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->password_entry));
    authentification(info);
    //--
    if (info->auth_client == 0) {
        pthread_cancel(login_msg_t);
        if (info->data->login_msg_flag) {
            gtk_widget_hide(info->data->login_msg);
            gtk_widget_hide(info->data->stop);
        }
        info->data->login_msg_flag = 1;
        gtk_widget_show(info->data->login_msg);
        gtk_widget_show(info->data->stop);
        pthread_create(&login_msg_t, 0, login_msg_thread, info);
    }
    else if(info->auth_client == 1) {
        init_general(info);
        init_menu(info);
        //init_profile(info);
        //--
    }  
}

void init_login(t_client_info *info) {
    //-login box
    info->data->login_msg_flag = 0;
    info->editing = -1;
    info->data->login_box = gtk_fixed_new ();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->login_box, 0, 0);
    //--
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/cz1.png", 400, 320, TRUE, NULL);
    info->data->stop = gtk_image_new_from_pixbuf(pixbuf);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), info->data->stop, 150, 140);
    //info->data->stop = gtk_image_new_from_file("img/stop1.png");
    //gtk_fixed_put(GTK_FIXED(info->data->login_box), info->data->stop, 60, 2);

    info->data->login_msg = gtk_label_new("Your login or password is invalid");
    gtk_widget_set_name (info->data->login_msg, "auth_fail");
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 170);
    gtk_box_pack_start (GTK_BOX (box), info->data->login_msg, TRUE, FALSE, 0);
    gtk_widget_show (box);
    //--table
    GtkWidget *table = gtk_grid_new();
    GtkWidget *title1 = gtk_label_new("Ucode");
    gtk_widget_show (title1);
    gtk_widget_set_name(title1, "title1");
    gtk_grid_attach (GTK_GRID (table), title1, 0, 0, 1, 1);
    GtkWidget *title2 = gtk_label_new("chat");
    gtk_widget_show (title2);
    gtk_widget_set_name(title2, "title2");
    gtk_grid_attach (GTK_GRID (table), title2, 1, 0, 1, 1);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 125);
    gtk_box_pack_start (GTK_BOX (box), table, TRUE, FALSE, 0);
    gtk_widget_show(table);
    gtk_widget_show (box);
    //--
    info->data->login_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->login_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->login_entry), "login");
    gtk_editable_select_region (GTK_EDITABLE (info->data->login_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->login_entry)));
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 200);
    gtk_box_pack_start (GTK_BOX (box), info->data->login_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->login_entry);
    gtk_widget_set_name(info->data->login_entry, "entry");

    info->data->password_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->password_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->password_entry), "password");
    gtk_editable_select_region (GTK_EDITABLE (info->data->password_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->password_entry)));
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 250);
    gtk_box_pack_start (GTK_BOX (box), info->data->password_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->password_entry);
    gtk_widget_set_name(info->data->password_entry, "entry");
                                                               
    GtkWidget *button = gtk_button_new_with_label("Sign in");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (enter_callback),info);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 300);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");

    button = gtk_button_new_with_label("Registration");
    g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (reg_callback),info);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 340);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");
    //--
    gtk_widget_show(info->data->login_box);
}

void init_window(t_client_info *info) {
    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path (provider,"my_style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                               GTK_STYLE_PROVIDER(provider),
                               GTK_STYLE_PROVIDER_PRIORITY_USER);
    info->data = (t_data *)malloc(sizeof(t_data));
    
    //--window
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
    //--main box
    info->data->main_box = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (info->data->window), info->data->main_box);
    gtk_widget_show (info->data->main_box);
}

int mx_login (t_client_info *info) {
    init_window(info);
    init_reg(info);
    init_login(info);

    gtk_main();
    return 0;
}

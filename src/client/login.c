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

void init_auth_fail(t_client_info *info, GtkWidget *screen) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/stop2.png", 600, 520, TRUE, NULL);
    info->data->stop = gtk_image_new_from_pixbuf(pixbuf);
    gtk_fixed_put(GTK_FIXED(screen), info->data->stop, 176, 106);

    info->data->login_msg = gtk_label_new("Your login or password is invalid");
    gtk_widget_set_name (info->data->login_msg, "auth_fail1");
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (screen), box, 0, 180);
    gtk_box_pack_start (GTK_BOX (box), info->data->login_msg, TRUE, FALSE, 0);
    gtk_widget_show (box);
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

void cancel_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Sign in");
    gtk_widget_hide(info->data->register_box);
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
    gtk_widget_show(info->data->login_box);
}

void init_main_title(t_client_info *info, GtkWidget *screen)  {
    GtkWidget *table = gtk_grid_new();
    GtkWidget *title1 = gtk_label_new("Ucode");
    gtk_widget_show (title1);
    gtk_widget_set_name(title1, "title1");
    gtk_grid_attach (GTK_GRID (table), title1, 0, 0, 1, 1);
    GtkWidget *title2 = gtk_label_new("chat");
    gtk_widget_show (title2);
    gtk_widget_set_name(title2, "title2");
    gtk_grid_attach (GTK_GRID (table), title2, 1, 0, 1, 1);
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (screen), box, 0, 125);
    gtk_box_pack_start (GTK_BOX (box), table, TRUE, FALSE, 0);
    gtk_widget_show(table);
    gtk_widget_show (box);
}

void init_reg_fail(t_client_info *info) {
    info->data->register_msg = gtk_label_new("Your login or password is invalid");
    gtk_widget_set_name (info->data->register_msg, "auth_fail1");
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 180);
    gtk_box_pack_start (GTK_BOX (box), info->data->register_msg, TRUE, FALSE, 0);
    gtk_widget_show (box);
}

void init_reg(t_client_info *info) {
    info->data->register_msg_t = NULL;
    info->data->register_box = gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->register_box, 0, 0);
    init_main_title(info, info->data->register_box);

    info->data->registration = (t_reg *)malloc(sizeof(t_reg));
    info->data->registration->login_entry = gtk_entry_new ();
    //--stop image
    GdkPixbuf *pixbuf1 = gdk_pixbuf_new_from_file_at_scale ("img/lr3.png", 400, 320, TRUE, NULL);
    GtkWidget *image1 = gtk_image_new_from_pixbuf(pixbuf1);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), image1, 300, 150);
    gtk_widget_show(image1);
    init_reg_fail(info);
    //--
    gtk_entry_set_max_length (GTK_ENTRY (info->data->registration->login_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->registration->login_entry), "Your login(6+ chars)");
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
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->registration->password_entry), "Your password(6+ chars)");
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
    //json_object_object_add(new_json, "login", json_object_new_string(info->login));
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

void logout_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    info->auth_client = 0;
    logout(info);
    gtk_window_set_title(GTK_WINDOW(info->data->window), "Login");
    gtk_entry_set_text(GTK_ENTRY(info->data->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->password_entry), "");
    gtk_widget_destroy(info->data->general_box);
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

void create_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show(info->data->create_room->main_box);
}

void search_callback (GtkWidget *widget, t_client_info *info) {
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

void show_search_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_show_all(info->data->search_box);
}

void close_search_callback (GtkWidget *widget, t_client_info *info) {
    (void)info;
    gtk_widget_hide(widget);
}

void init_search (t_client_info *info) { 
    info->data->search = NULL;
    info->data->search_box = gtk_event_box_new();
    gtk_widget_add_events (info->data->search_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (info->data->search_box), "button_press_event", G_CALLBACK (close_search_callback), info);
    gtk_widget_set_name (info->data->search_box, "search_exit");
    gtk_widget_set_size_request(info->data->search_box, 
                            gtk_widget_get_allocated_width (info->data->window), 
                            gtk_widget_get_allocated_height (info->data->window));
    gtk_fixed_put(GTK_FIXED(info->data->general_box),
                    info->data->search_box, 0, 0);
    GtkWidget *v_box = gtk_box_new(FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(v_box), GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_valign (v_box, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(info->data->search_box), v_box);
    GtkWidget *h_box = gtk_box_new(FALSE, 0);
    gtk_widget_set_halign (h_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (v_box), h_box, FALSE, FALSE, 0);  
    info->data->search_entry = gtk_entry_new ();
    gtk_widget_set_size_request(info->data->search_entry, 300, 45);
    gtk_entry_set_max_length (GTK_ENTRY (info->data->search_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->search_entry), "Anything you want or \"All\" for everything");
    gtk_editable_select_region (GTK_EDITABLE (info->data->search_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->search_entry)));
    gtk_box_pack_start (GTK_BOX (h_box), info->data->search_entry, FALSE, FALSE, 0);
    gtk_widget_set_name(info->data->search_entry, "entry");
    g_signal_connect(G_OBJECT(info->data->search_entry),"activate", G_CALLBACK(search_callback), info);
}

void close_creation_callback (GtkWidget *widget, GdkEventButton *event, t_client_info *info) {
    (void)widget;
    (void)event;
    gtk_widget_hide(info->data->create_room->main_box);
}

void close_creation_callback1 (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_hide(info->data->create_room->main_box);
}

void create_room_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    if (strcmp("",gtk_entry_get_text(GTK_ENTRY(info->data->create_room->name_entry))) != 0) {
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

GtkWidget *init_menu_fixed (GtkWidget *main_box) {
    GtkWidget *fixed = gtk_fixed_new();

    gtk_container_add(GTK_CONTAINER(main_box), fixed);
    gtk_widget_show(fixed);
    return fixed;
}

GtkWidget *init_menu_box (GtkWidget *fixed, int size) {
    GtkWidget *box = gtk_box_new(FALSE, 10);
    gtk_widget_set_size_request(box, size, -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(box), GTK_ORIENTATION_VERTICAL);
    gtk_fixed_put (GTK_FIXED (fixed), box, 0, 10);
    gtk_widget_show (box);
    return box;
}

GtkWidget *init_menu_main_box (t_client_info *info, GtkWidget *parent, char *style) {
    GtkWidget *main_box = gtk_event_box_new();

    (void) info;
    gtk_box_pack_start (GTK_BOX (parent), main_box, FALSE, FALSE, 0);
    gtk_widget_set_size_request(main_box, 150, -1);
    gtk_widget_set_name (main_box, style);
    gtk_widget_show(main_box);
    return main_box;
}

GtkWidget *init_menu_exit_box (t_client_info *info, GtkWidget *parent,
                        void (*callback) (GtkWidget *widget, GdkEventButton *event, t_client_info *info)) {
    GtkWidget *exit_box = gtk_event_box_new();

    gtk_box_pack_start (GTK_BOX (parent), exit_box, TRUE, TRUE, 0);
    gtk_widget_realize (exit_box);
    gtk_widget_add_events (exit_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (exit_box), "button_press_event", G_CALLBACK (callback), info);
    gtk_widget_set_name (exit_box, "menu_exit");
    gtk_widget_show(exit_box);
    return exit_box;
}

void init_create_entry (t_client_info *info, GtkWidget *box) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    info->data->create_room->name_entry = gtk_entry_new();
    gtk_widget_set_size_request(info->data->create_room->name_entry, 200, -1);
    gtk_entry_set_max_length (GTK_ENTRY (info->data->create_room->name_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->create_room->name_entry), "Write room name");
    gtk_editable_select_region (GTK_EDITABLE (info->data->create_room->name_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->create_room->name_entry)));
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->name_entry, TRUE, FALSE, 0);
    gtk_widget_show(info->data->create_room->name_entry);
    gtk_widget_show(box1);
}

void init_create_combo_box (t_client_info *info, GtkWidget *box) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    info->data->create_room->selection = gtk_combo_box_text_new();
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(info->data->create_room->selection), "0", "Public");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(info->data->create_room->selection), "1", "Private");
    gtk_combo_box_set_active (GTK_COMBO_BOX(info->data->create_room->selection), 0);
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->selection, TRUE, FALSE, 0);
    gtk_widget_set_size_request(info->data->create_room->selection, 200, -1);
    gtk_widget_show(info->data->create_room->selection);
    gtk_widget_show(box1);
}

void init_create_buttons (t_client_info *info, GtkWidget *box) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    info->data->create_room->create_button = gtk_button_new_with_label("OK");
    g_signal_connect(G_OBJECT(info->data->create_room->create_button), "clicked", G_CALLBACK(create_room_callback), info);
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->create_button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(info->data->create_room->create_button, 100, -1);
    gtk_widget_show(info->data->create_room->create_button);
    info->data->create_room->cancel_button = gtk_button_new_with_label("Cancel");
    g_signal_connect(G_OBJECT(info->data->create_room->cancel_button), "clicked", G_CALLBACK(close_creation_callback1), info);
    gtk_box_pack_start (GTK_BOX (box1), info->data->create_room->cancel_button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(info->data->create_room->cancel_button, 100, -1);
    gtk_widget_show(info->data->create_room->cancel_button);
    gtk_widget_show(box1);
}

void init_create (t_client_info *info) { 
    GtkWidget *main_box = NULL;
    GtkWidget *fixed = NULL;
    GtkWidget *exit_box = NULL;
    GtkWidget *box = NULL;

    info->data->create_room = (t_create *)malloc(sizeof(t_create));
    info->data->create_room->main_box = gtk_box_new(FALSE, 0);
    gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->create_room->main_box, 0, 0);
    gtk_widget_set_size_request(info->data->create_room->main_box, 
                                gtk_widget_get_allocated_width (info->data->window), 
                                gtk_widget_get_allocated_height (info->data->window));
    main_box = init_menu_main_box(info, info->data->create_room->main_box, "profile");
    fixed = init_menu_fixed (main_box);
    box = init_menu_box (fixed, 250);
    init_create_entry (info, box);
    init_create_combo_box (info, box);
    init_create_buttons (info, box);
    exit_box = init_menu_exit_box (info, info->data->create_room->main_box, 
                                    close_creation_callback);
}

void init_menu_button (t_client_info *info, GtkWidget *box, char *text,
                        void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);
    GtkWidget *button = gtk_button_new_with_label(text);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(callback), info);
    gtk_box_pack_start (GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
    gtk_widget_show(box1);
}

void init_menu (t_client_info *info) {
    GtkWidget *main_box = NULL;
    GtkWidget *fixed = NULL;
    GtkWidget *exit_box = NULL;
    GtkWidget *box = NULL;

    info->data->menu = gtk_box_new (FALSE, 0);
    gtk_fixed_put (GTK_FIXED(info->data->general_box), info->data->menu, 0, 0);
    gtk_widget_set_size_request (info->data->menu, gtk_widget_get_allocated_width (info->data->window), 
                                gtk_widget_get_allocated_height (info->data->window));
    main_box = init_menu_main_box (info, info->data->menu, "menu_main");
    fixed = init_menu_fixed (main_box);
    box = init_menu_box (fixed, 150);
    init_menu_button (info, box, "Create room", create_callback);
    init_menu_button (info, box, "Profile", profile_callback);
    init_menu_button (info, box, "Logout", logout_callback);
    exit_box = init_menu_exit_box(info, info->data->menu, close_menu_callback);
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

void init_stickers (t_client_info *info, GtkWidget *box) {
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

void init_vars_general(t_client_info *info) {
    info->data->profile = NULL;
    info->can_load = 1;
    info->data->rooms = NULL;
    info->data->current_room = 0;
}

void init_general_button_text (t_client_info *info, char *text, GtkWidget *box,
                        void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *button = gtk_button_new_with_label(text);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(callback), info);
    gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 0);
    gtk_widget_set_size_request(button, 75, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
}

void init_general_button (t_client_info *info, char *i_name, GtkWidget *box,
                        void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *button = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale (i_name, 20, 20, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    gtk_button_set_image(GTK_BUTTON(button), image);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK (callback), info);
    gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 0);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
}

void init_edit_button (t_client_info *info, char *i_name, GtkWidget *fixed_message) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale (i_name, 20, 20, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    info->data->edit_button = gtk_event_box_new ();
    gtk_widget_add_events (info->data->edit_button, GDK_BUTTON_PRESS_MASK);
    gtk_container_add (GTK_CONTAINER (info->data->edit_button), image);
    gtk_widget_show(image);
    g_signal_connect(G_OBJECT(info->data->edit_button), "button_press_event", G_CALLBACK(edit_cancel_callback), info);
    gtk_fixed_put(GTK_FIXED(fixed_message), info->data->edit_button, 670, 7);
}

void init_message_box (t_client_info *info, GtkWidget *box, 
                    void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *fixed_message = gtk_fixed_new();

    info->data->message_entry = gtk_entry_new ();
    gtk_entry_set_placeholder_text(GTK_ENTRY (info->data->message_entry), "Write something");
    gtk_entry_set_max_length(GTK_ENTRY (info->data->message_entry), 100);
    gtk_editable_select_region(GTK_EDITABLE (info->data->message_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->message_entry)));
    g_signal_connect(G_OBJECT(info->data->message_entry),"activate", G_CALLBACK(callback), info);
    gtk_box_pack_start (GTK_BOX (box), fixed_message, TRUE, TRUE, 0);
    gtk_fixed_put(GTK_FIXED(fixed_message), info->data->message_entry, 0, 0);
    gtk_widget_set_size_request(info->data->message_entry, 700, -1);
    gtk_widget_set_name(info->data->message_entry, "entry");
    gtk_widget_show(info->data->message_entry);
    gtk_widget_show(fixed_message);
    init_edit_button (info, "img/cancel.png", fixed_message);
}

void init_notebook (t_client_info *info, GtkWidget *box) {
    int n_rooms = json_object_array_length(info->rooms);

    info->data->notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK (info->data->notebook), GTK_POS_LEFT);
    gtk_widget_set_size_request(info->data->notebook, gtk_widget_get_allocated_width (info->data->window) -  20, 550);
    gtk_box_pack_start (GTK_BOX (box), info->data->notebook, TRUE, FALSE, 0);
    gtk_widget_show (info->data->notebook);
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(info->rooms, i);
        mx_push_room(info, room_data, i);   
    }
}

GtkWidget *init_bottom_box (t_client_info *info) {
    GtkWidget *box = gtk_box_new(FALSE, 10);

    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window) - 20, -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->general_box), box, 10, 570);
    gtk_widget_show (box);
    return box;
}

GtkWidget *init_top_box (t_client_info *info) {
    GtkWidget *box = gtk_box_new(FALSE, 0);

    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->general_box), box, 0, 10);
    gtk_widget_show (box);
    return box;
}

void init_general (t_client_info *info) {
    GtkWidget *box = NULL;

    info->data->general_box = gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->general_box, 0, 0);
    init_vars_general(info);
    box = init_bottom_box(info);
    init_general_button (info ,"img/a.png", box, menu_callback);
    init_message_box (info, box, send_callback);
    init_general_button_text (info ,"Send", box, send_callback);
    init_general_button (info ,"img/search.png", box, show_search_callback);
    init_general_button (info ,"img/file.png", box, choose_file_callback);
    init_stickers(info, box);
    box = init_top_box(info);
    init_notebook (info, box);
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

void enter_callback (GtkWidget *widget, t_client_info *info) {
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
        init_general (info);
        init_menu (info);
        init_search (info);
        init_create (info);
    }  
}

void init_login_entry (t_client_info *info, GtkWidget **entry, char *placeholder, int heigth) {
    *entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (*entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (*entry), placeholder);
    gtk_editable_select_region (GTK_EDITABLE (*entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (*entry)));
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, heigth);
    gtk_box_pack_start (GTK_BOX (box), *entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (*entry);
    gtk_widget_set_name(*entry, "entry");
}

void init_login_button (t_client_info *info, char *placeholder, int heigth, 
                        void (*callback) (GtkWidget *widget, t_client_info *info) ) {
    GtkWidget *button = gtk_button_new_with_label(placeholder);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (callback), info);
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, heigth);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");
}

void init_login(t_client_info *info) {
    info->data->login_msg_t = NULL;
    info->editing = -1;
    info->data->login_box = gtk_fixed_new ();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->login_box, 0, 0);
    init_auth_fail(info, info->data->login_box);
    init_main_title(info, info->data->login_box);
    init_login_entry(info, &info->data->login_entry, "login", 200);
    init_login_entry(info, &info->data->password_entry, "password", 250);
    init_login_button(info, "Sign in", 300, enter_callback);
    init_login_button(info, "Registration", 340, reg_callback);
    gtk_widget_show(info->data->login_box);
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
    init_reg(info);
    init_login(info);

    gtk_main();
    return 0;
}

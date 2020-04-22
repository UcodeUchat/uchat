#include "uchat.h"

pthread_t watcher;
pthread_t login_msg_t;
int send_flag = 0;

void sleep_ms (int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

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

void *watcher_thread (void *data) {
    t_data *n_data = (t_data *)data;
    gint current_room = gtk_notebook_get_current_page(GTK_NOTEBOOK(n_data->notebook));
    t_room *room = find_room(n_data->rooms, current_room);

    while (1) {
        sleep_ms(100);
        if (send_flag) {
            sleep_ms(100);
            gtk_adjustment_set_value(room->Adjust, 
                gtk_adjustment_get_upper(room->Adjust) - 
                gtk_adjustment_get_page_size(room->Adjust));
            send_flag = 0;
        }
    }
    return 0;
}


t_room *create_room(void *name, int position) {
    t_room *node =  (t_room *)malloc(sizeof(t_room));
    node->name = strdup(name);
    node->position = position;
    node->id = position; //tmp
    
    node->next = NULL;
    return node;
}

void push_room(t_room **list, void *name, int id) {
    t_room *tmp;
    t_room *p;

    if (!list)
        return;
    tmp = create_room(name, id);  // Create new
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
    int position = gtk_notebook_get_current_page(GTK_NOTEBOOK(info->data->notebook));
    t_room *room = find_room(info->data->rooms, position);
    info->data->current_room = room->id;
    mx_process_message_in_client(info);
    gtk_entry_set_text(GTK_ENTRY(info->data->message_entry), "");
    send_flag = 1;
}

int validation (char *login, char *password, char *repeat_password) {
    (void)login;
    (void)password;
    (void)repeat_password;
    return 1;
}

void send_data_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    char *login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->login_entry));
    char *password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->password_entry));
    char *repeat = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->registration->repeat_password_entry));
    if (validation(login, password, repeat)) {
        gtk_widget_hide(info->data->register_box);
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
        gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
        gtk_widget_show(info->data->login_box);
        //succes thread
    }
    else {

    }
}

void cancel_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    gtk_widget_hide(info->data->register_box);
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(info->data->registration->repeat_password_entry), "");
    gtk_widget_show(info->data->login_box);
}

void reg_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;

    gtk_widget_hide(info->data->login_box);
    info->data->register_box = gtk_fixed_new ();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->register_box, 0, 0);
    gtk_widget_show (info->data->register_box);

    info->data->registration = (t_reg *)malloc(sizeof(t_reg));
    info->data->registration->login_entry = gtk_entry_new ();
 
    gtk_entry_set_max_length (GTK_ENTRY (info->data->registration->login_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->registration->login_entry), "Write your login");
    gtk_editable_select_region (GTK_EDITABLE (info->data->registration->login_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->registration->login_entry)));
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 100);
    gtk_box_pack_start (GTK_BOX (box), info->data->registration->login_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->registration->login_entry);
    gtk_widget_set_name(info->data->registration->login_entry, "entry");

    info->data->registration->password_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->registration->password_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->registration->password_entry), "Write your password");
    gtk_editable_select_region (GTK_EDITABLE (info->data->registration->password_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->registration->password_entry)));
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 150);
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
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 200);
    gtk_box_pack_start (GTK_BOX (box), info->data->registration->repeat_password_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->registration->repeat_password_entry);
    gtk_widget_set_name(info->data->registration->repeat_password_entry, "entry");
                                                    
    GtkWidget *button = gtk_button_new_with_label("Register");
    g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (send_data_callback),info);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 250);
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
    gtk_fixed_put (GTK_FIXED (info->data->register_box), box, 0, 290);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");
}

void authentification(t_client_info **info, t_package *p) {
    // fprintf(stderr, "socket = [%d]\n", (*info)->socket);
    char *answer = mx_strnew(1);
    // char *done = NULL;
    // char *massage не нужна, я сделал пока так, ибо при NULL - упадет strlen
    mx_send_message_from_client(*info, p, " ");
    mx_memset(p->data, 0, sizeof(p->data));
    recv(p->client_sock, answer, 2, MSG_WAITALL);
    // read(p->client_sock, answer, 1);
    fprintf(stderr, "ANSWER = [%s]\n", answer);
    if (atoi(answer) == 1)
        (*info)->auth_client = 1;
    else
        (*info)->auth_client = 0;
    mx_strdel(&answer);
}

// void authentification(t_client_info *info) {
//     if (strcmp(info->login, "rrr") == 0)
//         info->auth_client = 0;
//     else
//         info->auth_client = 1;
// }

void enter_callback (GtkWidget *widget, t_client_info *info) {
    (void)widget;
    t_package *p = mx_create_new_package();
    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path (provider,"my_style.css", NULL);
    //--auth
    info->login = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->login_entry));
    info->password = (char *)gtk_entry_get_text(GTK_ENTRY(info->data->password_entry));
    strncat(p->login, info->login, sizeof(p->login) - 1);
    strncat(p->password, info->password, sizeof(p->password) - 1);
    p->type = MX_AUTH_TYPE;
    p->client_sock = info->socket;
    authentification(&info, p);
    //authentification(info);
    if (!info->auth_client) {
        pthread_cancel(login_msg_t);
        if (info->data->login_msg_flag) {
            gtk_widget_hide(info->data->login_msg);
            gtk_widget_hide(info->data->stop);
        }
        info->data->login_msg = gtk_label_new("Your login or password is invalid");
        gtk_widget_set_name (info->data->login_msg, "auth_fail");
        GtkStyleContext *context = gtk_widget_get_style_context (info->data->login_msg);
        gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
        info->data->login_msg_flag = 1;
        gtk_fixed_put(GTK_FIXED(info->data->login_box), info->data->login_msg, 196, 75);
        gtk_widget_show(info->data->login_msg);
        gtk_widget_show(info->data->stop);
        pthread_create(&login_msg_t, 0, login_msg_thread, info);
    }
    else {
        if (info->data->login_msg_flag)
            gtk_widget_hide(info->data->login_msg);
        GtkStyleContext *context;
        info->data->login_msg = gtk_label_new("Welcome, daddy");
        info->data->login_msg_flag = 1;
        gtk_widget_hide(info->data->login_box);
        gtk_window_set_title(GTK_WINDOW(info->data->window), "Uchat");
        info->data->rooms = NULL;
        info->data->current_room = 0;
        info->data->general_box = gtk_fixed_new();
        gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->general_box, 0, 0);
        gtk_widget_show(info->data->general_box);
        //--message
        info->data->message_entry = gtk_entry_new ();
        gtk_entry_set_placeholder_text(GTK_ENTRY (info->data->message_entry), "Write something");
        gtk_entry_set_max_length(GTK_ENTRY (info->data->message_entry), 255);
        gtk_editable_select_region(GTK_EDITABLE (info->data->message_entry),
                                    0, gtk_entry_get_text_length (GTK_ENTRY (info->data->message_entry)));
        g_signal_connect(G_OBJECT(info->data->message_entry),"activate", G_CALLBACK(send_callback), info);
        gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->message_entry, 65, 350);
        gtk_widget_set_size_request(info->data->message_entry, 445, -1);
        gtk_widget_show(info->data->message_entry);
        //--
        //--Menu button
        info->data->menu_button = gtk_button_new();
        //g_signal_connect(G_OBJECT(info->data->send_button), "clicked", G_CALLBACK(send_callback), info);
        gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->menu_button, 10, 350);
        gtk_widget_set_size_request(info->data->menu_button, 50, -1);
        gtk_widget_show(info->data->menu_button);
        //--
        //--Send button
        info->data->send_button = gtk_button_new_with_label("Send");
        g_signal_connect(G_OBJECT(info->data->send_button), "clicked", G_CALLBACK(send_callback), info);
        gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->send_button, 520, 350);
        gtk_widget_set_size_request(info->data->send_button, 75, -1);
        gtk_widget_show(info->data->send_button);
        //--
        //--File selection
        info->data->file_button = gtk_button_new();
        GtkWidget *image = gtk_image_new_from_file("c.png");
        gtk_button_set_image(GTK_BUTTON(info->data->file_button), image);
        //g_signal_connect(G_OBJECT(info->data->file_button), "clicked", G_CALLBACK(choose_file_callback), info->data);
        gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->file_button, 600, 350);
        gtk_widget_show(info->data->file_button);
        //--
        //--notebook
        info->data->notebook = gtk_notebook_new();
        gtk_notebook_set_tab_pos(GTK_NOTEBOOK (info->data->notebook), GTK_POS_LEFT);
        gtk_fixed_put(GTK_FIXED(info->data->general_box), info->data->notebook, 10, 20);
        gtk_widget_set_size_request(info->data->notebook, 630, 320);
        for (int i = 0; i < 4; i++) {
            char *str;
            if (i == 0) 
                str = strdup("General123456789123456");
            else {
                str = strdup("Room  ");
                str[5] = i + 48;
            }
            push_room(&info->data->rooms, str, i);
            t_room *room = find_room(info->data->rooms, i);
            room->room_box = gtk_box_new(FALSE, 0);
            GtkWidget *fixed = gtk_fixed_new();
            gtk_box_pack_start (GTK_BOX (room->room_box), fixed, TRUE, TRUE, 0);
            gtk_widget_show(fixed);
            gtk_widget_set_name (room->room_box, "room_box");
            context = gtk_widget_get_style_context (room->room_box);
            gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
            gtk_widget_show(room->room_box);
            //--table
            GtkWidget *table = gtk_grid_new();
            gtk_fixed_put (GTK_FIXED(fixed), table, 0, 10);
            gtk_widget_show(table);
            //--
            GtkWidget *full_name = gtk_label_new(str);
            gtk_widget_set_name (full_name, "title");
            context = gtk_widget_get_style_context (full_name);
            gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
            gtk_grid_attach (GTK_GRID (table), full_name, 0, 1, 1, 1);

            gtk_widget_set_size_request(full_name, 515, -1);
            gtk_widget_show(full_name);
            room->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
            gtk_container_set_border_width(GTK_CONTAINER(room->scrolled_window), 5);
            gtk_grid_attach (GTK_GRID (table), room->scrolled_window, 0, 2, 1, 10);
            
            gtk_widget_set_size_request(room->scrolled_window, 515, 295);
            gtk_widget_show(room->scrolled_window);
            room->Adjust = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(room->scrolled_window));
            if (strlen(str) > 15) {
                str = strndup(str, 12);
                strcat(str, "...");
            }
            GtkWidget *label = gtk_label_new(str);
            gtk_notebook_append_page(GTK_NOTEBOOK(info->data->notebook), room->room_box, label);
            room->list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
            room->messagesTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(room->list));
            GtkTreeViewColumn *column;
            GtkCellRenderer *renderer;
            renderer = gtk_cell_renderer_text_new();
            g_object_set(G_OBJECT (renderer),"foreground", "red", NULL);
            column = gtk_tree_view_column_new_with_attributes("Author", renderer,
                                                              "text", 0,
                                                              NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(room->messagesTreeView), column);
            renderer = gtk_cell_renderer_text_new ();
            column = gtk_tree_view_column_new_with_attributes("Message", renderer,
                                                              "text", 1,
                                                              NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(room->messagesTreeView), column);
            gtk_container_add(GTK_CONTAINER(room->scrolled_window), room->messagesTreeView);
            gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(room->messagesTreeView), FALSE);
            gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(room->messagesTreeView), FALSE);
            gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(room->messagesTreeView)), GTK_SELECTION_NONE);
            gtk_widget_show(room->messagesTreeView);
        }
        gtk_widget_show(info->data->notebook);
        //--
        //--Send thread
        pthread_create(&watcher, 0, watcher_thread, info->data);
        //--
    }
    
}

int mx_login (t_client_info *info) {
    GtkStyleContext *context;
    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_path (provider,"my_style.css", NULL);

    info->data = (t_data *)malloc(sizeof(t_data));
    
    //--window
    info->data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable (GTK_WINDOW (info->data->window), FALSE);
    gtk_widget_set_size_request (GTK_WIDGET (info->data->window), 650, 407);
    gtk_widget_show (info->data->window);
    //sleep_ms(150);
    //gtk_container_set_border_width (GTK_CONTAINER (info->data->window), 1);
    gtk_window_set_title (GTK_WINDOW (info->data->window), "Sign in");
    g_signal_connect (G_OBJECT (info->data->window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect_swapped (G_OBJECT (info->data->window), "delete_event",
                              G_CALLBACK (gtk_widget_destroy), 
                              G_OBJECT (info->data->window));
    //--
    //--main box
    info->data->main_box = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (info->data->window), info->data->main_box);
    gtk_widget_show (info->data->main_box);
    //--
    //-login box
    info->data->login_msg_flag = 0;
    info->data->login_box = gtk_fixed_new ();
    gtk_fixed_put(GTK_FIXED(info->data->main_box), info->data->login_box, 0, 0);
    gtk_widget_show (info->data->login_box);
    //--
    gtk_widget_set_name(info->data->window, "back");
    context = gtk_widget_get_style_context (info->data->window);
    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
    //--
    //--table
    GtkWidget *table = gtk_grid_new();
    gtk_fixed_put(GTK_FIXED(info->data->login_box), table, 230, 40);
    gtk_widget_show(table);
    GtkWidget *title1 = gtk_label_new("Ucode");
    gtk_widget_show (title1);
    gtk_widget_set_name(title1, "title1");
    context = gtk_widget_get_style_context (title1);
    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_grid_attach (GTK_GRID (table), title1, 0, 0, 1, 1);
    GtkWidget *title2 = gtk_label_new("chat");
    gtk_widget_show (title2);
    gtk_widget_set_name(title2, "title2");
    context = gtk_widget_get_style_context (title2);
    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_grid_attach (GTK_GRID (table), title2, 1, 0, 1, 1);
    //--
    info->data->stop = gtk_image_new_from_file("stop2.png");
    gtk_fixed_put(GTK_FIXED(info->data->login_box), info->data->stop, 60, 2);

    info->data->login_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->login_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->login_entry), "login");
    gtk_editable_select_region (GTK_EDITABLE (info->data->login_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->login_entry)));
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 100);
    gtk_box_pack_start (GTK_BOX (box), info->data->login_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->login_entry);
    gtk_widget_set_name(info->data->login_entry, "entry");
    context = gtk_widget_get_style_context (info->data->login_entry);
    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);

    info->data->password_entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (info->data->password_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->password_entry), "password");
    gtk_editable_select_region (GTK_EDITABLE (info->data->password_entry),
                                0, gtk_entry_get_text_length (GTK_ENTRY (info->data->password_entry)));
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 150);
    gtk_box_pack_start (GTK_BOX (box), info->data->password_entry, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_show (info->data->password_entry);
    gtk_widget_set_name(info->data->password_entry, "entry");
    context = gtk_widget_get_style_context (info->data->password_entry);
    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
                                                               
    GtkWidget *button = gtk_button_new_with_label("Sign in");
    g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (enter_callback),info);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 200);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    //gtk_button_set_relief (GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_widget_set_name(button, "entry");
    context = gtk_widget_get_style_context (button);
    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);

    button = gtk_button_new_with_label("Registration");
    g_signal_connect (G_OBJECT (button), "clicked",G_CALLBACK (reg_callback),info);
    box = gtk_box_new(FALSE, 0);
    gtk_widget_set_size_request(box, gtk_widget_get_allocated_width (info->data->window), -1);
    gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
    gtk_fixed_put (GTK_FIXED (info->data->login_box), box, 0, 240);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, FALSE, 0);
    gtk_widget_show (box);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_show (button);
    gtk_widget_set_name(button, "entry");
    context = gtk_widget_get_style_context (button);
    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
    //--
    gtk_main();
    return 0;
}
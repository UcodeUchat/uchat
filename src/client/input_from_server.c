#include "uchat.h"

t_message *mx_find_message(t_message *messages, int id) {
   t_message *head = messages;
   t_message *node = NULL;

    while (head != NULL) {
        if (head->id == id) {
            node = head;
            break;
        }
        head = head->next;
    }
    return node;
}

typedef struct s_all {
    t_room *room;
    GtkWidget *widget;
}               t_all;

void focus_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    gtk_widget_show(message->menu);
}

void focus1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    gtk_widget_hide(message->menu);
}

void file_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file_pressed");
    mes->message = mx_find_message(mes->room->messages, mes->id);
    mx_download_file_from_server(mes);
}

void file1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file_hover");
}

void file_notify_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    t_message *message = mx_find_message(mes->room->messages, mes->id);
    gtk_widget_show(message->menu);
    gtk_widget_set_name(widget, "file_hover");
}

void file_notify1_callback(GtkWidget *widget, GdkEventButton *event, t_mes *mes) {
    (void)widget;
    (void)event;
    (void)mes;
    gtk_widget_set_name(widget, "file");
}

void show_message(t_all *data) {
    gtk_widget_show(data->widget);
    sleep_ms(100);
    gtk_adjustment_set_value(data->room->Adjust, 
                            gtk_adjustment_get_upper(data->room->Adjust) - 
                            gtk_adjustment_get_page_size(data->room->Adjust) + 2.0);
}

t_message *create_message(t_client_info *info, t_room *room, json_object *new_json) {
    t_message *node =  (t_message *)malloc(sizeof(t_message));
    int id = json_object_get_int(json_object_object_get(new_json, "id"));
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    int add_info = json_object_get_int(json_object_object_get(new_json, "add_info"));
    const char *login = json_object_get_string(json_object_object_get(new_json, "login"));
    const char *message = json_object_get_string(json_object_object_get(new_json, "data"));

    node->id = id;
    node->data = strdup(message);

    t_mes *mes = (t_mes *)malloc(sizeof(t_mes));
    mes->info = info;
    mes->room = room;
    mes->id = id;
    node->h_box = gtk_box_new(FALSE, 0);
    // gtk_widget_show(node->h_box);

    gtk_box_pack_start (GTK_BOX (room->message_box), node->h_box, FALSE, FALSE, 0);
    GtkWidget *general_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(general_box);
    //-events
    GtkWidget *event = gtk_event_box_new();
    gtk_widget_show(event);
    gtk_widget_add_events (event, GDK_ENTER_NOTIFY_MASK);
    g_signal_connect (G_OBJECT (event), "enter_notify_event", G_CALLBACK (focus_callback), mes);
    g_signal_connect (G_OBJECT (event), "leave_notify_event", G_CALLBACK (focus1_callback), mes);
    gtk_box_pack_start (GTK_BOX (general_box), event, FALSE, FALSE, 0);
    //--
    GtkWidget *box = gtk_box_new(FALSE, 0);
    gtk_widget_show(box);
    gtk_container_add (GTK_CONTAINER (event), box);
    gtk_widget_set_name(general_box, "message");
    gtk_container_set_border_width(GTK_CONTAINER(box), 1);

    GtkWidget *left_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(left_box);
    gtk_widget_set_size_request(left_box, 10, -1);
    gtk_box_pack_start(GTK_BOX (box), left_box, FALSE, FALSE, 0);
    GtkWidget *main_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(main_box);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(main_box), GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX (box), main_box, FALSE, FALSE, 0);
    GtkWidget *right_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(right_box);
    gtk_widget_set_size_request(right_box, 15, -1);
    gtk_box_pack_start(GTK_BOX (box), right_box, FALSE, FALSE, 0);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale ("img/options.png", 20, 40, TRUE, NULL);
    node->menu = gtk_image_new_from_pixbuf(pixbuf);

    GtkWidget *box1 = gtk_box_new(FALSE, 0);
    gtk_widget_show(box1);
    //gtk_container_set_border_width(GTK_CONTAINER(box1), 1);
    GtkWidget *label1 = gtk_label_new(login);
    gtk_box_pack_start(GTK_BOX (main_box), box1, FALSE, FALSE, 0);
    gtk_widget_show(label1);
    
    if (add_info == 1) {
        GtkWidget *box2 = gtk_event_box_new();
        gtk_widget_show(box2);
        gtk_box_pack_start (GTK_BOX (main_box), box2, FALSE, FALSE, 0);
        GtkWidget *label2 = gtk_label_new(message);
        gtk_widget_show(label2);
        gtk_container_add (GTK_CONTAINER (box2), label2);
        gtk_widget_set_name(box2, "file");
        gtk_widget_add_events (box2, GDK_BUTTON_PRESS_MASK);
        g_signal_connect (G_OBJECT (box2), "button_press_event", G_CALLBACK (file_callback), mes);
        g_signal_connect (G_OBJECT (box2), "button_release_event", G_CALLBACK (file1_callback), mes);
        gtk_widget_add_events (box2, GDK_ENTER_NOTIFY_MASK);
        g_signal_connect (G_OBJECT (box2), "enter_notify_event", G_CALLBACK (file_notify_callback), mes);
        g_signal_connect (G_OBJECT (box2), "leave_notify_event", G_CALLBACK (file_notify1_callback), mes);
    }
    else {
        GtkWidget *box2 =  gtk_box_new(FALSE, 0);
        gtk_widget_show(box2);
        gtk_box_pack_start (GTK_BOX (main_box), box2, FALSE, FALSE, 0);
        GtkWidget *label2 = gtk_label_new(message);
        gtk_widget_show(label2);
        gtk_box_pack_start (GTK_BOX (box2), label2, FALSE, FALSE, 0);
    }
    if (user_id == info->id) {
        gtk_box_pack_end(GTK_BOX (right_box), node->menu, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX (box1), label1, FALSE, FALSE, 0);
        gtk_box_pack_end (GTK_BOX (node->h_box), general_box, FALSE, FALSE, 0);
        t_all *data = (t_all *)malloc(sizeof(t_all));
        data->widget = node->h_box;
        data->room = room; 
        gdk_threads_add_idle ((GSourceFunc)show_message, data);
        // gtk_widget_show(node->h_box);
        // sleep_ms(50);
    //     gtk_adjustment_set_value(room->Adjust, 
    //                             gtk_adjustment_get_upper(room->Adjust) - 
    //                             gtk_adjustment_get_page_size(room->Adjust) + 2.0);
    }
    else {
        gtk_box_pack_start(GTK_BOX (box1), label1, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (node->h_box), general_box, FALSE, FALSE, 0);
        //gdk_threads_add_idle (show_message, node->h_box);
        // gtk_widget_show(node->h_box);
    }
    node->next = NULL;
    return node;
}

void push_message(t_client_info *info, t_room *room, json_object *new_json) {
    t_message *tmp;
    t_message *p;
    t_message **list = &room->messages;

    if (!list)
        return;
    tmp = create_message(info, room, new_json);  // Create new
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

t_room *mx_find_room(t_room *rooms, int id) {
   t_room *head = rooms;
   t_room *node = NULL;

    while (head != NULL) {
        if (head->id == id) {
            node = head;
            break;
        }
        head = head->next;
    }
    return node;
}

void input_message(t_client_info *info, json_object *new_json) {
    int room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    t_room *room = mx_find_room(info->data->rooms, room_id);

    push_message(info, room, new_json);           
    t_room *head = info->data->rooms;
    while (head != NULL) {
        if (head && head->position < room->position)
            head->position = head->position + 1;
        head = head->next;
    }
    room->position = 0;
    gtk_notebook_reorder_child(GTK_NOTEBOOK(info->data->notebook), room->room_box, 0);
}

void input_authentification(t_client_info *info, json_object *new_json) {
    int type = json_object_get_int(json_object_object_get(new_json, "type"));
    int user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));

    if ((*info).auth_client == 0){
        fprintf(stderr, "ANSWER = [%d]\n", type);
        if (type == 4) {
            info->id = user_id;
            (*info).auth_client = 1;
            json_object_deep_copy(json_object_object_get(new_json, "rooms"), &info->rooms, NULL);
        }
        else{
            (*info).auth_client = 0;
        }
        fprintf(stderr, "(*info).auth_client = [%d]\n", (*info).auth_client);
        (*info).responce = 1;
    }
}

int mx_run_function_type_in_client(t_client_info *info, json_object *obj) {
    int type = json_object_get_int(json_object_object_get(obj, "type"));
// tmp
    if (type != MX_FILE_DOWNLOAD_TYPE)
        mx_print_json_object(obj, "mx_process_input_from_server");
    printf("New_package! Type:%d\n", type);
//
    if (type == MX_FILE_DOWNLOAD_TYPE) {
        mx_save_file_in_client(info, obj);
    }
    else if (type == MX_AUTH_TYPE_V || type == MX_AUTH_TYPE_NV) {
        input_authentification(info, obj);
    }
    else if (type == MX_REG_TYPE_V || type == MX_REG_TYPE_NV) {
        //input_registration(info, obj);
    }
    else if (type == MX_MSG_TYPE) {
        input_message(info, obj);
    }
    return 0;
}

void *mx_process_input_from_server(void *taken_info) {
    t_client_info *info = (t_client_info *)taken_info;
    int rc;
    char buffer[2048];
    json_tokener *tok = json_tokener_new();
    enum json_tokener_error jerr;
    json_object *new_json;

    while (1) { // read all input from server
        rc = tls_read(info->tls_client, buffer, sizeof(buffer));    // get json
        if (rc == -1)
            mx_err_exit("tls connection error\n");
        if (rc != 0) {
            new_json = json_tokener_parse_ex(tok, buffer, rc);
            jerr = json_tokener_get_error(tok);
            if (jerr == json_tokener_success) {
                mx_run_function_type_in_client(info, new_json);
            }
            else if (jerr != json_tokener_continue) {
                fprintf(stderr, "Error: %s\n", json_tokener_error_desc(jerr));
            }
            json_object_put(new_json);
        }
    }
    return NULL;
}

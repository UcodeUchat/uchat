#include "uchat.h"

int mx_notebook_prepend(t_note *note) {
    gtk_notebook_append_page(GTK_NOTEBOOK
                (note->notebook), note->box, note->label);
    gtk_notebook_reorder_child(GTK_NOTEBOOK
                (note->notebook), note->box, note->position);
    gtk_notebook_set_current_page (GTK_NOTEBOOK
                (note->notebook), note->position);
    return 0;
}

static char *trim_name (t_room *room) {
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

static void show_room (t_client_info *info, t_room *room, int position) {
    char *str = trim_name (room);
    GtkWidget *label = gtk_label_new(str);
    t_note *note = (t_note *)malloc(sizeof(t_note));

    note->notebook = info->data->notebook;
    note->box = room->room_box;
    note->label = label;
    note->position = position;
    g_idle_add ((GSourceFunc)mx_notebook_prepend, note);
}

static t_room *create_room (t_client_info *info, 
                            json_object *room_data, int position) {
    t_room *room =  (t_room *)malloc(sizeof(t_room));
    t_all *data = (t_all *)malloc(sizeof(t_all));

    mx_init_room (info, room, position, room_data);
    mx_init_room_data (info, room ,room_data, data);
    mx_init_room_box (room);
    mx_init_room_menu (room, data);
    mx_init_room_header (room);
    mx_init_room_window (room);
    mx_init_room_messsage_box (room);
    mx_load_room_history (data);
    show_room (info, room, position);
    return room;
}

void mx_push_room(t_client_info *info, json_object *room_data, int position) {
    t_room *tmp;
    t_room *p;
    t_room **list = &info->data->rooms;

    if (!list)
        return;
    tmp = create_room(info, room_data, position);  // Create new
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

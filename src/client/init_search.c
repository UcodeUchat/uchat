#include "uchat.h"

static void close_search_callback (GtkWidget *widget, t_client_info *info) {
    (void)info;
    gtk_widget_hide(widget);
}

static void config_serach_box (t_client_info *info) {
    info->data->search = NULL;
    info->data->search_box = gtk_event_box_new();
    gtk_widget_add_events (info->data->search_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (info->data->search_box), "button_press_event", 
        G_CALLBACK (close_search_callback), info);
    gtk_widget_set_name (info->data->search_box, "search_exit");
    gtk_widget_set_size_request(info->data->search_box, 
                            gtk_widget_get_allocated_width (info->data->window), 
                            gtk_widget_get_allocated_height (info->data->window));
    gtk_fixed_put(GTK_FIXED(info->data->general_box),
                    info->data->search_box, 0, 0);
    info->data->search_entry = gtk_entry_new ();
    gtk_widget_set_size_request(info->data->search_entry, 300, 45);
    gtk_entry_set_max_length (GTK_ENTRY (info->data->search_entry), 50);
    gtk_entry_set_placeholder_text (GTK_ENTRY (info->data->search_entry), 
        "Anything you want or \"All\" for everything");
    gtk_editable_select_region (GTK_EDITABLE (info->data->search_entry),
                    0, gtk_entry_get_text_length (GTK_ENTRY (info->data->search_entry)));
}

void mx_init_search (t_client_info *info) { 
    GtkWidget *v_box = NULL;
    GtkWidget *h_box = NULL;

    config_serach_box(info);
    v_box = gtk_box_new(FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE(v_box), GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_valign (v_box, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(info->data->search_box), v_box);
    h_box = gtk_box_new(FALSE, 0);
    gtk_widget_set_halign (h_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (v_box), h_box, FALSE, FALSE, 0);  
    gtk_box_pack_start (GTK_BOX (h_box), info->data->search_entry, FALSE, FALSE, 0);
    gtk_widget_set_name(info->data->search_entry, "entry");
    g_signal_connect(G_OBJECT(info->data->search_entry),"activate", 
                     G_CALLBACK(mx_search_callback), info);
}

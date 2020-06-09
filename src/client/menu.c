#include "uchat.h"

static void init_menu_button (t_client_info *info, GtkWidget *box, char *text,
                        void (*callback) (GtkWidget *widget, t_client_info *info)) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);
    GtkWidget *button = gtk_button_new_with_label(text);

    gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (box), box1, TRUE, FALSE, 0);
    g_signal_connect(G_OBJECT(button), "clicked", 
                    G_CALLBACK(callback), info);
    gtk_box_pack_start (GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
    gtk_widget_show(box1);
}

static GtkWidget *init_delete_menu (t_client_info *info) {
    GtkWidget *answer_menu = gtk_menu_new();
    GtkWidget *item_question = gtk_menu_item_new_with_label("Delete or not?");
    GtkWidget *item_no = gtk_menu_item_new_with_label("Nope");
    GtkWidget *item_yep = gtk_menu_item_new_with_label("Yep");

    gtk_widget_set_sensitive (item_question, FALSE);
    gtk_widget_show(item_question);
    gtk_menu_shell_append (GTK_MENU_SHELL (answer_menu), item_question);
    gtk_widget_show(item_no);
    gtk_menu_shell_append (GTK_MENU_SHELL (answer_menu), item_no);
    gtk_widget_show(item_yep);
    gtk_menu_shell_append (GTK_MENU_SHELL (answer_menu), item_yep);
    g_signal_connect (G_OBJECT (item_yep), "activate", 
                    G_CALLBACK (mx_yep_callback), info);
    return answer_menu;
}

static void init_delete_button (t_client_info *info, GtkWidget *box, char *text,
                        void (*callback) (GtkWidget *widget, GtkWidget *answer_menu)) {
    GtkWidget *box1 = gtk_box_new(FALSE, 0);
    GtkWidget *button = gtk_button_new_with_label(text);
    GtkWidget *answer_menu = init_delete_menu(info);

    gtk_widget_set_halign(box1, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX (box), box1, TRUE, FALSE, 0);
    g_signal_connect(G_OBJECT(button), "clicked", 
                    G_CALLBACK(callback), answer_menu);
    gtk_box_pack_start(GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_set_size_request(button, 100, -1);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
    gtk_widget_show(box1);
}

void mx_init_menu (t_client_info *info) {
    GtkWidget *main_box = NULL;
    GtkWidget *fixed = NULL;
    GtkWidget *exit_box = NULL;
    GtkWidget *box = NULL;

    info->data->menu = gtk_box_new (FALSE, 0);
    gtk_fixed_put (GTK_FIXED(info->data->general_box), info->data->menu, 0, 0);
    gtk_widget_set_size_request (info->data->menu, 
                                gtk_widget_get_allocated_width (info->data->window), 
                                gtk_widget_get_allocated_height (info->data->window));
    main_box = mx_init_menu_main_box (info, info->data->menu, "menu_main");
    fixed = mx_init_menu_fixed (main_box);
    box = mx_init_menu_box (fixed, 150);
    init_menu_button (info, box, "Create room", mx_create_callback);
    init_menu_button (info, box, "Profile", mx_profile_callback);
    init_menu_button (info, box, "Logout", mx_logout_callback);
    init_delete_button (info, box, "Delete acc", mx_delete_acc_callback);
    exit_box = mx_init_menu_exit_box(info, info->data->menu, mx_close_menu_callback);
}

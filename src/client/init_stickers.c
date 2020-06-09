#include "uchat.h"

static char *get_extention (char *path) {
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

static void init_sticker_item (t_client_info *info, char *path, 
                                struct dirent  *ds1, GtkWidget *sub_menu) {
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
    g_signal_connect (G_OBJECT (item), "activate", 
                      G_CALLBACK (mx_item_callback), stik);
}

static void init_sticker_pack (t_client_info *info, char *path, 
                                struct dirent  *ds, GtkWidget *menu) {
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

static void show_sticker_button (GtkWidget *menu, GtkWidget *box) {
    GtkWidget *button = gtk_button_new();
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale 
                        ("img/smile.png", 20, 20, TRUE, NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    
    gtk_button_set_image(GTK_BUTTON(button), image);
    g_signal_connect(G_OBJECT(button), "clicked", 
                     G_CALLBACK(mx_choose_sticker_callback), menu);
    gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 0);
    gtk_widget_set_name(button, "entry");
    gtk_widget_show(button);
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
    show_sticker_button(menu, box);  
}

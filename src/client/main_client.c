#include "uchat.h"

void create_download_folder() {
    struct stat st;// = {0};

    if (stat(MX_SAVE_FOLDER_IN_CLIENT, &st) == -1) {
        mkdir(MX_SAVE_FOLDER_IN_CLIENT, 0700);
    }
}

int main(int argc, char **argv) {
    t_client_info *info = NULL;

    if (argc != 3) {
        mx_printerr("usage: uchat [ip_adress] [port]\n");
        return -1;
    }
    gtk_init (&argc, &argv);


    const gchar *nano_str;
    guint major, minor, micro, nano;
    gst_init (&argc, &argv);
    gst_version (&major, &minor, &micro, &nano);
    if (nano == 1)
    nano_str = "(CVS)";
    else if (nano == 2)
    nano_str = "(Prerelease)";
    else
    nano_str = "";
    printf ("This program is linked against GStreamer %d.%d.%d %s\n",
        major, minor, micro, nano_str);

    
    info = (t_client_info *)malloc(sizeof(t_client_info));
    memset(info, 0, sizeof(t_client_info));
    (*info).argc = argc;
    (*info).argv = argv;
    (*info).ip = argv[1];
    (*info).port = (uint16_t) atoi(argv[2]);
    (*info).tls_client = NULL;
    (*info).responce = 0;
    create_download_folder();
    if (mx_start_client(info)) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

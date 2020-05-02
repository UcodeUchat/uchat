#include "uchat.h"

void create_download_folder() {
    struct stat st = {0};

    if (stat(MX_SAVE_FOLDER_IN_SERVER, &st) == -1) {
        mkdir(MX_SAVE_FOLDER_IN_SERVER, 0700);
    }
}

static void init_db(t_server_info *info) {
    int status = sqlite3_open(MX_PATH_TO_DB, &(info->db));

    if (status != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(info->db));
        exit(1);
    }
}

int main(int argc, char **argv) {
    t_server_info *info = NULL;
    struct servent *servent;


// test json
    struct json_object *jobj;
    char *str = "{ \"msg-type\": [ \"0xdeadbeef\", \"irc log\" ], \
\"msg-from\": { \"class\": \"soldier\", \"name\": \"Wixilav\" }, \
\"msg-to\": { \"class\": \"supreme-commander\", \"name\": \"[Redacted]\" }, \
\"msg-log\": [ \
   \"soldier: Boss there is a slight problem with the piece offering to humans\", \
   \"supreme-commander: Explain yourself soldier!\", \
   \"soldier: Well they don't seem to move anymore...\", \
   \"supreme-commander: Oh snap, I came here to see them twerk!\" \
   ] \
}";
    printf("str:\n---\n%s\n---\n\n", str);


    jobj = json_tokener_parse(str);
    printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

    if (argc != 2)
        mx_err_exit("usage: chat_server [port]\n");
    info = (t_server_info *)malloc(sizeof(t_server_info));
    memset(info, 0, sizeof(t_server_info));
    (*info).argc = argc;
    (*info).argv = argv;
    (*info).port = (uint16_t) atoi(argv[1]);
    if ((*info).port < 1024)
        mx_err_exit("port id must be minimum 1024\n");
    if ((servent = getservbyport((*info).port, "tcp")) != NULL
        || (servent = getservbyport((*info).port, "udp")) != NULL)
        mx_err_exit("port was taken by another process\n");
    pthread_mutex_init(&((*info).mutex), NULL);
    printf("Configuring .\n");
    // if (mx_set_daemon(info) == -1) {
    //     printf("error = %s\n", strerror(errno));
    //     return -1;
    // }
    printf("Configuring demon ++...\n");
    init_db(info);
    create_download_folder();
    if (mx_start_server(info) == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    sqlite3_close(info->db);
    return 0;
}



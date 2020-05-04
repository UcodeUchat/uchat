#include "uchat.h"

#define KEY1 "id"
#define KEY2 "type"   // char type
#define KEY3 "piece"   // char piece
#define KEY4 "user_id"  // int used_id
#define KEY5 "room_id"  // int room_id
#define KEY6 "add_info"  // int add_info
#define KEY7 "login"  // char[]
#define KEY8 "password"  // char[]
#define KEY9 "data"  // char[]
//#define KEY5 "time"


json_object *mx_package_to_json(t_package *package) {
    json_object *new_json;

    new_json = json_object_new_object();
    json_object_object_add(new_json, KEY2, json_object_new_int(package->type));
    json_object_object_add(new_json, KEY3, json_object_new_int(package->piece));
    json_object_object_add(new_json, KEY4, json_object_new_int(package->user_id));
    json_object_object_add(new_json, KEY5, json_object_new_int(package->room_id));
    json_object_object_add(new_json, KEY6, json_object_new_int(package->add_info));
    json_object_object_add(new_json, KEY7, json_object_new_string(package->login));
    json_object_object_add(new_json, KEY8, json_object_new_string(package->password));
    json_object_object_add(new_json, KEY9, json_object_new_string(package->data));
    return new_json;
}

t_package *mx_json_to_package(json_object *new_json) {
    t_package *new_package = malloc(MX_PACKAGE_SIZE);

    new_package->type = json_object_get_int(json_object_object_get(new_json, "type"));
    new_package->piece = json_object_get_int(json_object_object_get(new_json, "piece"));
    new_package->user_id = json_object_get_int(json_object_object_get(new_json, "user_id"));
    new_package->room_id = json_object_get_int(json_object_object_get(new_json, "room_id"));
    new_package->add_info = json_object_get_int(json_object_object_get(new_json, "add_info"));
    strcpy(new_package->login, json_object_get_string(json_object_object_get(new_json, "login")));
    strcpy(new_package->password, json_object_get_string(json_object_object_get(new_json, "password")));
    strcpy(new_package->data, json_object_get_string(json_object_object_get(new_json, "data")));
    return new_package;
}


const char *mx_message_to_json_string(t_client_info *info, char *message) {
    struct json_object *mesg;

    mesg = json_object_new_object();
    json_object_object_add(mesg, KEY1, json_object_new_int(100));
    json_object_object_add(mesg, KEY2, json_object_new_string(info->login));
    json_object_object_add(mesg, KEY3, json_object_new_int(1));
    json_object_object_add(mesg, KEY4, json_object_new_string(message));
    json_object_object_add(mesg, KEY5, json_object_new_string(mx_curr_time()));
    return  json_object_to_json_string(mesg);
}

void mx_print_json_object(struct json_object *jobj, const char *msg) {
    printf("\n%s: \n", msg);
    printf("---\n%s\n---\n", json_object_to_json_string(jobj));
}

struct json_object * mx_find_something(struct json_object *jobj, const char *key) {
    struct json_object *tmp;

    json_object_object_get_ex(jobj, key, &tmp);

    return tmp;
}

/*
 * Swap the first and second elements of the array received.
 * Again, more complicated in real life...
 */
void mx_glitch_in_the_matrix(struct json_object *jobj) {
    struct json_object *pos1;
    struct json_object *pos2;

    mx_print_json_object(jobj, "Before the glitch in the matrix");

    pos1 = json_object_array_get_idx(jobj, 0);
    pos2 = json_object_array_get_idx(jobj, 1);

    // json_object_array_put_idx decrement the refcount if you replace an
    // element, but we don't whant that.
    json_object_array_put_idx(jobj, 0, json_object_get(pos2));
    json_object_array_put_idx(jobj, 1, json_object_get(pos1));
    mx_print_json_object(jobj, "After the glitch in the matrix");
}



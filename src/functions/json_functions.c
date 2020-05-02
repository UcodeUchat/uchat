#include "uchat.h"

#define KEY1 "id"
#define KEY2 "user"
#define KEY3 "room"
#define KEY4 "text"
#define KEY5 "time"



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



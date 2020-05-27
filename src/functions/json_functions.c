#include "uchat.h"

json_object *mx_create_basic_json_object(int type) {
    json_object *new_object = json_object_new_object();

    json_object_object_add(new_object, "type", json_object_new_int(type));
    return new_object;
}

void mx_print_json_object(struct json_object *jobj, const char *msg) {
    printf("\n%s: \n", msg);
    printf("---\n%s\n---\n", json_object_to_json_string(jobj));
}

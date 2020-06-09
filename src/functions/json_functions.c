#include "uchat.h"

json_object *mx_create_basic_json_object(int type) {
    json_object *new_object = mx_js_n_o();

    mx_js_o_o_add(new_object, "type", mx_js_n_int(type));
    return new_object;
}

void mx_print_json_object(struct json_object *jobj, const char *msg) {
    printf("\n%s: \n", msg);
    printf("---\n%s\n---\n", mx_js_o_to_js_str(jobj));
}

#include "uchat.h"

int32_t mx_js_g_int(const struct json_object *obj) {
    return json_object_get_int(obj);
}

struct json_object *mx_js_n_int(int32_t i) {
    return json_object_new_int(i);
}

int mx_js_s_int(struct json_object *obj, int new_value) {
    return json_object_set_int(obj, new_value);
}

struct json_object *mx_js_n_o() {
    return json_object_new_object();
}

struct json_object *mx_js_n_str(const char *s) {
    return json_object_new_string(s);
}

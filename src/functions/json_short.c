#include "uchat.h"

int mx_js_o_o_add(struct json_object *obj, const char *key,
                  struct json_object *val) {
    return json_object_object_add(obj, key, val);
}

struct json_object *mx_js_o_o_get(const struct json_object *obj,
                                  const char *key) {
    return json_object_object_get(obj, key);
}

json_bool mx_js_o_o_get_ex(const struct json_object *obj, const char *key,
                           struct json_object **value) {
    return json_object_object_get_ex(obj, key, value);
}

const char *mx_js_g_str(json_object *jso) {
    return json_object_get_string(jso);
}

int mx_js_g_str_len(const struct json_object *obj) {
    return json_object_get_string_len(obj);
}

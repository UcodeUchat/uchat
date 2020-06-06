#include "uchat.h"

void mx_send_empty_json(struct tls *tls_socket) {
    json_object *json_obj = mx_create_basic_json_object(MX_EMPTY_JSON);
    const char *json_string = json_object_to_json_string(json_obj);

    tls_send(tls_socket, json_string, strlen(json_string));
}

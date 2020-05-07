#include "stdio.h"
#include "./json/json.h"

void mx_print_json_object(struct json_object *jobj, const char *msg) {
    printf("\n%s: \n", msg);
    printf("---\n%s\n---\n", json_object_to_json_string(jobj));
}

int main(){
	json_object *js =  json_object_new_array();

	json_object *new = json_object_object_add()
	mx_print_json_object(js, "array");
}




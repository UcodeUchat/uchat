/*
 * Reference counting example. The idea is to make a json_object res composed from
 * parts of obj1 and obj2.
 *
 * clang -Wall -g -I/usr/include/json-c/ -o json_refcount json_refcount.c -ljson-c
 */
#include "./json/json.h"
#include <stdio.h>

#define KEY1 "peace-code"
#define KEY2 "death-ray-code"   // char type
#define KEY3 "code"   // char type
#define KEY4 "authorization"

/*
 * Just a utility function.
 */
void print_json_object(struct json_object *jobj, const char *msg) {
	printf("\n%s: \n", msg);
	printf("---\n%s\n---\n", json_object_to_json_string(jobj));
}

/*
 * In real life the operations done here are way more complicated, but it's
 * only an example.
 */
struct json_object * find_something(struct json_object *jobj, const char *key) {
	struct json_object *tmp;

	json_object_object_get_ex(jobj, key, &tmp);

	return tmp;
}

/*
 * Swap the first and second elements of the array received.
 * Again, more complicated in real life...
 */
void glitch_in_the_matrix(struct json_object *jobj) {
	struct json_object *pos1, *pos2;

	print_json_object(jobj, "Before the glitch in the matrix");

	pos1 = json_object_array_get_idx(jobj, 0);
	pos2 = json_object_array_get_idx(jobj, 1);

	// json_object_array_put_idx decrement the refcount if you replace an
	// element, but we don't whant that.
	json_object_array_put_idx(jobj, 0, json_object_get(pos2));
	json_object_array_put_idx(jobj, 1, json_object_get(pos1));

	print_json_object(jobj, "After the glitch in the matrix");
}

int main() {
	struct json_object *obj1, *obj2, *res, *obj3, *obj4, *sub_obj1, *sub_obj2, *tmp;

	char *json_string = "{\"authorization\": {\"type\": 1, \"login\": \"neo\", \"password\": \"1\" }}";

    json_object  *new_json = json_tokener_parse(json_string);
    print_json_object(new_json, "new_json ");

	// Creation and print of obj1 and obj2
	obj1 = json_object_new_object();
	obj2 = json_object_new_object();
	json_object_object_add(obj1, KEY1, json_object_new_int(1234));
	json_object_object_add(obj2, KEY2, json_object_new_int(4321));
	print_json_object(obj1, "obj1 in plaintext");
	print_json_object(obj2, "obj2 in plaintext");


    obj3 = json_object_new_object();
    obj4 = json_object_new_object();
    json_object_object_add(obj3, KEY1, json_object_new_int(555));
    json_object_object_add(obj4, KEY1, json_object_new_int(777));
    json_object_object_add(obj1, KEY3, obj3);
    json_object_object_add(obj1, KEY2, obj4);

    print_json_object(obj1, "obj1 ");


    json_object *o_get = json_object_object_get(new_json, KEY4
    );
    if (o_get)
        print_json_object(o_get, "obj get");


	// Extraction of the interesting parts of obj1 and obj2
	sub_obj1 = find_something(obj1, "peace-code");
	sub_obj2 = find_something(obj2, "death-ray-code");

	res = json_object_new_array();

	// We are going to use those in res so we need to increment the
	// refcount
//	json_object_array_add(res, json_object_get(sub_obj1));
//	json_object_array_add(res, json_object_get(sub_obj2));

//	glitch_in_the_matrix(res);

//	tmp = json_object_array_get_idx(res, 0);
	printf("\n[>] Unlocking peace with code at index 0 of res: %d\n", json_object_get_int(tmp));

	// We can safely put all of these objects
	json_object_put(obj1);
	json_object_put(obj2);
	json_object_put(res);

	return 0;
}


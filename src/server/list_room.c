#include "uchat.h"

#define KEY10 "rooms_user"
#define KEY11 "rooms_name"

static int get_rooms(void *, int argc, char **argv, char **col_name) {

	(void)argc;
	(void)col_name;
	(void)argv;
	(void)rooms;
	return 0;
}

void mx_get_rooms(t_server_info *i, json_object *js) {
	char *req = mx_strnew(1024);
	int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
	json_object *array = json_object_new_array();

	json_object_object_add(js, KEY10, array);
	// json_object_array_add(array, json_object_new_int(1));
	// json_object_array_add(array, json_object_new_int(2));
	// json_object_array_add(array, json_object_new_int(3));
	// json_object_array_add(array, json_object_new_int(4));
	// json_object_array_add(array, json_object_new_string("General"));
    sprintf(req, "select distinct user_id, rooms.id, name from room_user, rooms \
    			where rooms.id=room_user.room_id and user_id=%d;", user_id);
    printf("%s\n", req);
    if (sqlite3_exec(i->db, req, get_rooms, js, 0) != SQLITE_OK) {
        printf("Work rooms\n");
    }
    mx_print_json_object(js, "js_in_room");
    mx_strdel(&req);
}































































// void print_list(t_server_room *r) {
// 	if (r) {
// 		t_server_room *tmp = r;

// 		while (tmp) {
// 			fprintf(stderr, "room->name = %s -- ", tmp->name);
// 			fprintf(stderr, "room->room_id = [%d]\n", tmp->room_id);
// 			tmp = tmp->next;
// 		}
// 	}
// }

// t_server_room *mx_create_list(char *name, int id) {
// 	t_server_room *new_node = NULL;

// 	if (name) {
// 		new_node = (t_server_room *)malloc(sizeof(t_server_room));
// 		new_node->room_id = id;
// 		new_node->name = mx_strdup(name);
// 		new_node->next = NULL;
// 	}
// 	return new_node;
// }

// void mx_add_room(t_server_room **room, char *name, int id){
// 	t_server_room *new_node = mx_create_list(name, id);

// 	if (*room) {
// 		t_server_room *tmp = *room;

// 		while (tmp->next)
// 			tmp = tmp->next;
// 		tmp->next = new_node;
// 	}
// 	else
// 		*room = new_node;
// }

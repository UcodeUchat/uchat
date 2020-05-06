#include "uchat.h"

#define KEY10 "rooms"


static int get_rooms(void *array, int argc, char **argv, char **col_name) {
	json_object *room = json_object_new_object();
	json_object *name = json_object_new_string(argv[2]);
	json_object *id = json_object_new_int(atoi(argv[1]));

	(void)argc;
	(void)col_name;
	json_object_object_add(room, "room_id", id);
	json_object_object_add(room, "name", name);
	json_object_array_add((struct json_object *)array, room);
	return 0;
}

void mx_get_rooms(t_server_info *i, json_object *js) {
	char *req = mx_strnew(1024);
	int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
	json_object *array = json_object_new_array();

	json_object_object_add(js, KEY10, array);
    sprintf(req, "select distinct user_id, rooms.id, name from room_user, rooms \
    			where rooms.id=room_user.room_id and user_id=%d;", user_id);
    printf("%s\n", req);
    if (sqlite3_exec(i->db, req, get_rooms, array, 0) != SQLITE_OK) {
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

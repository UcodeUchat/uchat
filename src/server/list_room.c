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

static int get_rooms_data(void *messages, int argc, char **argv, char **col_name) {
	json_object *message = json_object_new_object();
	json_object *id = json_object_new_int(atoi(argv[0]));
	json_object *user_id = json_object_new_int(atoi(argv[1]));
	json_object *data = NULL;
	json_object *login = json_object_new_string(argv[8]);
	json_object *add_info = NULL;

	if (argv[0] == NULL) {
		return 1;
	}
	(void)argc;
	(void)col_name;
	if (strcmp(argv[4], "file") == 0) {
		char *extention = strdup(argv[3]);

	    while (mx_get_char_index(extention, '.') >= 0) {
	        char *tmp = strdup(extention + mx_get_char_index(extention, '.') + 1);
	        free(extention);
	        extention = strdup(tmp);
	        free(tmp); 
	    }
	    if (strcmp(extention, "jpg") == 0 || strcmp(extention, "png") == 0) {
	    	data = json_object_new_string(argv[3]);
			add_info = json_object_new_int(2);
	    }
	    else {
			data = json_object_new_string(argv[3] /*+ 20*/);
			add_info = json_object_new_int(1);
		}
	}
	else {
		data = json_object_new_string(argv[3]);
		add_info = json_object_new_int(0);
	}
	json_object_object_add(message, "id", id);
	json_object_object_add(message, "user_id", user_id);
	json_object_object_add(message, "data", data);
	json_object_object_add(message, "login", login);
	json_object_object_add(message, "add_info", add_info);
	json_object_array_add((struct json_object *)messages, message);
	return 0;
}

void mx_get_rooms(t_server_info *info, json_object *js) {
	char *req = mx_strnew(1024);
	int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
	json_object *array = json_object_new_array();

	json_object_object_add(js, KEY10, array);
    sprintf(req, "select distinct user_id, rooms.id, name from room_user, rooms \
    			where rooms.id=room_user.room_id and user_id=%d;", user_id);
    printf("%s\n", req);
    if (sqlite3_exec(info->db, req, get_rooms, array, 0) != SQLITE_OK) {
        printf("Work rooms\n");
    }
    int n_rooms = json_object_array_length(array);
    for (int i = 0; i < n_rooms; i++) {
        json_object *room_data = json_object_array_get_idx(array, i);
        json_object *messages = json_object_new_array();
        int room_id = json_object_get_int(json_object_object_get(room_data, "room_id"));
        char *req1 = mx_strnew(1024);

        json_object_object_add(room_data, "messages", messages);
        sprintf(req1, "SELECT *  FROM msg_history, users \
        		where room_id = %d and users.id = msg_history.user_id order by msg_history.id desc limit 15;", room_id);
        if (sqlite3_exec(info->db, req1, get_rooms_data, messages, 0) != SQLITE_OK) {
        	printf("Work rooms data\n");
    	}
    	mx_strdel(&req1);
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

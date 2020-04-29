#include "uchat.h"

void print_list(t_server_room *r) {
	if (r) {
		t_server_room *tmp = r;

		while (tmp) {
			fprintf(stderr, "room->name = %s -- ", tmp->name);
			fprintf(stderr, "room->room_id = [%d]\n", tmp->room_id);
			tmp = tmp->next;
		}
	}
}

t_server_room *mx_create_list(char *name, int id) {
	t_server_room *new_node = NULL;

	if (name) {
		new_node = (t_server_room *)malloc(sizeof(t_server_room));
		new_node->room_id = id;
		new_node->name = mx_strdup(name);
		new_node->next = NULL;
	}
	return new_node;
}

void mx_add_room(t_server_room **room, char *name, int id){
	t_server_room *new_node = mx_create_list(name, id);

	if (*room) {
		t_server_room *tmp = *room;

		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_node;
	}
	else
		*room = new_node;
}

static int get_rooms(void *rooms, int argc, char **argv, char **col_name) {
	t_server_room **new = (t_server_room **)rooms;

	(void)argc;
	(void)col_name;
	mx_add_room(new, argv[2], atoi(argv[1]));
	return 0;
}

void mx_get_rooms(t_server_info *i, t_package **p) {
	t_server_room *rooms = NULL;
	char *req = mx_strnew(1024);

    sprintf(req, "select distinct user_id, rooms.id, name from room_user, rooms \
    			where rooms.id=room_user.room_id and user_id=%d;", (*p)->user_id);
    printf("%s\n", req);
    if (sqlite3_exec(i->db, req, get_rooms, &rooms, 0) != SQLITE_OK) {
        printf("Work rooms\n");
    }
    (*p)->rooms = rooms;
    print_list((*p)->rooms);
    mx_strdel(&req);
}

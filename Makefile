NAME_S = uchat_server
NAME_C = uchat


INC = uchat.h

SRC_SERVER = main_server.c \
    start_server.c \
    set_daemon.c \
    server_worker.c \
    work_with_db.c \
    request_for_bd.c \
	run_function_type.c \
	input_message.c \
	input_file.c \

SRC_CLIENT = main_client.c \
    start_client.c \
	send_message.c \


SRC_HELP = err_exit.c \
    functions.c \
	package.c \

OBJ_SERVER = main_server.o \
    start_server.o \
    set_daemon.o \
    server_worker.o \
    work_with_db.o \
    request_for_bd.o \
	run_function_type.c \
	input_message.o \
	input_file.o \

OBJ_CLIENT = main_client.o \
    start_client.o \
	send_message.o \

OBJ_HELP = err_exit.o \
    functions.o \
	package.o \

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

all: install clean

install:
	#@make install -C libmx
	@cp $(addprefix inc/, $(INC)) .
	@cp $(addprefix src/server/, $(SRC_SERVER)) .
	@cp $(addprefix src/client/, $(SRC_CLIENT)) .
	@cp $(addprefix src/functions/, $(SRC_HELP)) .
	@clang $(CFLAGS) -c $(SRC_SERVER) $(SRC_HELP) -I $(INC)
	@clang $(CFLAGS) -c $(SRC_CLIENT) $(SRC_HELP) -I $(INC)
	@clang $(CFLAGS) libmx/libmx.a $(OBJ_SERVER) $(OBJ_HELP) -o $(NAME_S) -lsqlite3
	@clang $(CFLAGS) libmx/libmx.a $(OBJ_CLIENT) $(OBJ_HELP) -o $(NAME_C) -lsqlite3
	@mkdir -p obj
	@mv $(OBJ_SERVER) $(OBJ_CLIENT) $(OBJ_HELP) ./obj

uninstall: clean
	#@make uninstall -C libmx
	@rm -rf $(NAME_S) $(NAME_C)

clean:
	#@make clean -C libmx
	@rm -rf $(INC)
	@rm -rf $(SRC) $(SRC_SERVER) $(SRC_CLIENT) $(SRC_HELP)
	@rm -rf $(OBJ) $(OBJ_SERVER) $(OBJ_CLIENT) $(OBJ_HELP)
	@rm -rf ./obj

reinstall: uninstall install

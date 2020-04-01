NAME_S = uchat_server
NAME_C = uchat


INC = uchat.h

SRC_SERVER = main_server.c \
    start_server.c \
    set_daemon.c \
    server_worker.c \
    functions.c

SRC_CLIENT = main_client.c \
    start_client.c


SRC_HELP = err_exit.c

OBJ_SERVER = main_server.o \
    start_server.o \
    set_daemon.o \
    server_worker.o \
    functions.o

OBJ_CLIENT = main_client.o \
    start_client.o

OBJ_HELP = err_exit.o

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
	@clang $(CFLAGS) libmx/libmx.a $(OBJ_SERVER) $(OBJ_HELP) -o $(NAME_S)
	@clang $(CFLAGS) libmx/libmx.a $(OBJ_CLIENT) $(OBJ_HELP) -o $(NAME_C)
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

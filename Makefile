NAME_S = uchat_server
NAME_C = uchat

#colors
RESET			= \033[m
RED         	= \033[1;31m
GREEN       	= \033[01;38;05;46m
YELLOW      	= \033[01;38;05;226m
BLUE        	= \033[03;38;05;21m
VIOLET      	= \033[01;38;05;201m
CYAN        	= \033[1;36m
WHITE       	= \033[01;38;05;15m


SRCD = src
INCD = inc
OBJD = obj

LMXD	=	libmx
LMXA:=	$(LMXD)/libmx.a
LMXI:=	$(LMXD)/inc

INCS = inc/uchat.h

SRC_SERVER = main_server.c \
	start_server.c \
	set_daemon.c \
	server_worker.c \
	work_with_db.c \
	request_for_bd.c \
	run_function_type.c \
	process_message_in_server.c \
	process_file_in_server.c \
	authorization.c \
	work_with_socket_list.c \
	work_with_socket_list_2.c \
	work_with_files_in_server.c \
	request_for_rooms.c \
	list_room.c \

SRC_CLIENT = main_client.c \
	start_client.c \
	send_message.c \
	send_file.c \
	input_from_server.c \
	login.c \
	process_file_in_client.c \
	work_with_files_in_client.c \

# SRC_HELP = $(wildcard *.c)
SRC_HELP = err_exit.c \
	functions.c \
	package.c \
	cryptographic_hash_f.c \

INCLUDE = -I $(LBMXD) \
	-I $(INCD) \

SRCS_SERVER = $(addprefix $(SRCD)/server/, $(SRC_SERVER))
SRCS_CLIENT = $(addprefix $(SRCD)/client/, $(SRC_CLIENT))
SRCS_HELP = $(addprefix $(SRCD)/functions/, $(SRC_HELP))

OBJS_SERVER = $(addprefix $(OBJD)/, $(SRC_SERVER:%.c=%.o))
OBJS_CLIENT = $(addprefix $(OBJD)/, $(SRC_CLIENT:%.c=%.o))
OBJS_HELP = $(addprefix $(OBJD)/, $(SRC_HELP:%.c=%.o))

JSON_C_DIR=./json
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -g -fsanitize=address

LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c
CFLAGS += -I$(JSON_C_DIR)/include/json-c

TLSFLAGS =  -lcrypto -lssl -ltls
SQLFLAGS = -lsqlite3

all: install

server: $(LMXA) $(NAME_S)

$(NAME_S): $(OBJS_SERVER) $(OBJS_HELP)
	@clang $(CFLAGS) `pkg-config --cflags --libs gtk+-3.0` libmx/libmx.a   $(OBJS_SERVER) $(OBJS_HELP) -o $@ $(TLSFLAGS) $(SQLFLAGS)
	@printf "\r\33[2K$@\t   \033[32;1mcreated\033[0m\n"

$(OBJD)/%.o: src/server/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
	@printf "\r\33[2K  \033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

$(OBJS_SERVER): | $(OBJD)

$(OBJS_HELP): | $(OBJD)

$(OBJD):
	@mkdir -pv $@

# $(LMXA):
# 	@make -sC $(LMXD)

client: $(LMXA) $(NAME_C)

$(NAME_C): $(OBJS_CLIENT) $(OBJS_HELP)
	@clang $(CFLAGS) `pkg-config --cflags --libs gtk+-3.0` libmx/libmx.a $(OBJS_CLIENT) $(OBJS_HELP) -o $@ $(TLSFLAGS) $(SQLFLAGS)
	@printf "\r\33[2K$@\t\t  \033[32;1mcreated\033[0m\n"

$(OBJD)/%.o: src/client/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
	@printf "\r\33[2K  \033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "


$(OBJD)/%.o: src/functions/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
	@printf "\r\33[2K  \033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

$(OBJS_CLIENT): | $(OBJD)

$(OBJS_HELP): | $(OBJD)

$(OBJD):
	@mkdir -pv $@

# $(LMXA):
# 	@make -sC $(LMXD)

install: server client

clean:
	@rm -rf $(OBJD)
	@printf "$(OBJD)\t   \033[31;1mdeleted\033[0m\n"

uninstall: clean
#     @make uninstall -C libmx
	@rm -rf $(NAME_S) $(NAME_C)
#     @make -C ./libmx/ uninstall
	@printf "$(NAME_S)\t   \033[31;1muninstalled\033[0m\n"
	@printf "$(NAME_C)\t   \033[31;1muninstalled\033[0m\n"

reinstall: uninstall install
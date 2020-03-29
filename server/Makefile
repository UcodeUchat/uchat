NAME = uchat_server
SRCS = src/main.c \
	src/init_info.c \
	src/daemonize.c \
	src/start_server.c \
	src/start_client.c \
	src/work_with_clients_list.c \
	src/read_client_input.c \
	src/check_client_requests.c \
	\
	src/help_funcs/err_exit.c \
	\
	src/client_authorization/chat_connect.c \
	src/client_authorization/sign_in.c \
	src/client_authorization/sign_up.c \
	\
	src/client_in_chat/chat_menu.c \
	src/client_in_chat/message_received.c \

OBJ_DIR = obj
OBJS = $(SRCS:.c=.o)
FLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -lsqlite3
HEADER = inc/uchat.h
LIB = libmx/libmx.a

all: install clean

install: $(NAME)
$(NAME): $(LIB) $(OBJS)
	@clang $(FLAGS) $(OBJS) $(LIB) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRCS)
	@mkdir -p $(OBJ_DIR)
	@clang $(FLAGS) -o $@ -c $< $(HEADER)

$(LIB):
	@make -C ./libmx/
clean:
	@rm -rf $(OBJS)
	@make -C ./libmx/ clean

uninstall: clean
	@rm -f $(NAME)
	@make -C ./libmx/ uninstall

reinstall: uninstall install

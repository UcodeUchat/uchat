NAME = uchat
SRCS = src/main.c \
	src/init_info.c \
	src/start_client.c \
	src/input_from_client.c \
	src/input_from_server.c \
	\
	src/help_funcs/err_exit.c \

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

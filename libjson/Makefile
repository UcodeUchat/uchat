NAME = libjsonc.a

SRC = arraylist.c \
	debug.c \
	json_c_version.c \
	json_object.c \
	json_object_iterator.c \
	json_pointer.c \
	json_tokener.c \
	json_util.c \
	json_visit.c \
	libjson.c \
	linkhash.c \
	printbuf.c \
	random_seed.c \
	strerror_override.c \

SRCS = $(addprefix src/, $(SRC))
OBJ_DIR = obj
OBJS = $(SRCS:.c=.o)
CFLAGS = -std=c11 -Wall 
HEADER = json.h

all: install

install: $(NAME)

$(NAME): $(OBJS)
	@ar -rcs $(NAME) $(OBJS)
	@mkdir -p $(OBJ_DIR)
	@mv $(OBJS) $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRCS)
	@clang $(CFLAGS) -o $@ -c $< $(HEADER)

uninstall:
	@rm -rf $(NAME)

clean:
	@rm -rf ./obj

reinstall: uninstall install

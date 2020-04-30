NAME = json.a

INC = json.h \
	json.h \
	arraylist.h \
	debug.h \
	json_c_version.h \
	json_inttypes.h \
	json_object.h \
	json_object_iterator.h \
	json_pointer.h \
	json_tokener.h \
	json_types.h \
	json_util.h \
	json_visit.h \
	linkhash.h \
	printbuf.h \
	config.h \
	json_config.h

OBJD = obj

SRC = json.c \
      arraylist.c \
      debug.c \
      json_c_version.c \
      json_inttypes.c \
      json_object.c \
      json_object_iterator.c \
      json_pointer.c \
      json_tokener.c \
      json_types.c \
      json_util.c \
      json_visit.c \
      linkhash.c \
      printbuf.c \

OBJ = $(addprefix $(OBJD)/, $(SRC:%.c=%.o))
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

all: install clean

install:
	@#cp $(addprefix src/, $(SRC)) .
	@#cp $(addprefix inc/, $(INC)) .
	@clang $(CFLAGS) -c $(SRC)
	@ar -rcs $(NAME) $(OBJ)
	@mkdir -p obj
	@mv $(OBJ) ./obj

uninstall: clean
	@rm -rf $(NAME)

clean:
	@rm -rf $(INC) $(SRC) ./obj

reinstall: uninstall install

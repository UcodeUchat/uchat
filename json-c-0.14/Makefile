NAME = libjson-c.a

INC = json.h

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
	strerror_override.c


OBJ = arraylist.o \
	debug.o \
	json_c_version.o \
	json_object.o \
	json_object_iterator.o \
	json_pointer.o \
	json_tokener.o \
	json_util.o \
	json_visit.o \
	libjson.o \
	linkhash.o \
	printbuf.o \
	random_seed.o \
	strerror_override.o

CFLAGS = -std=c11 -Wall

# CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

all: install clean

install:
	@cp $(addprefix src/, $(SRC)) .
# 	@cp $(addprefix inc/, $(INC)) .
	@clang $(CFLAGS) -c $(SRC)
	@ar rcs $(NAME) $(OBJ)
	@mkdir -p obj
	@mv $(OBJ) ./obj

uninstall: clean
	@rm -rf $(NAME)

clean:
	@rm -rf $(SRC) ./obj

reinstall: uninstall install

NAME_S = uchat_server
NAME_C = uchat


INC = uchat.h

SRC_SERVER = main_server.c \
             mx_server_worker.c \
	     	functions.c

SRC_CLIENT = main_client.c

OBJ_SERVER = main_server.o \
             mx_server_worker.o \
	     	functions.o

OBJ_CLIENT = main_client.o

#CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic
CFLAGS = -std=c11

LIBRESSL_A = ./libressl/tls/.libs/libtls.a \
			 ./libressl/ssl/.libs/libssl.a \
			 ./libressl/crypto/.libs/libcrypto.a

LIBRESSL_H = \
			-I ./libressl/include/tls.h \
			-I ./libressl/include/openssl \
			-I ./libressl/include/pqueue.h \
			-I ./libressl/tls \
			-I ./libressl/ssl \
			-I ./libressl/crypto

all: install clean clean

install:
	#@make install -C libmx
	@cp $(addprefix inc/, $(INC)) .
	@cp $(addprefix src/server/, $(SRC_SERVER)) .
	@cp $(addprefix src/client/, $(SRC_CLIENT)) .
	@clang $(CFLAGS)  $(LIBRESSL_H) $(LIBRESSL_A) $(SRC_SERVER)  -I $(INC)
	@clang $(CFLAGS)  $(LIBRESSL_H) $(LIBRESSL_A) $(SRC_CLIENT)  -I $(INC)
	@clang $(CFLAGS) libmx/libmx.a  $(OBJ_SERVER) -o $(NAME_S)
	@clang $(CFLAGS) libmx/libmx.a   $(OBJ_CLIENT)  -o $(NAME_C)
	@mkdir -p obj
	@mv $(OBJ_SERVER) $(OBJ_CLIENT) ./obj

uninstall: clean
	#@make uninstall -C libmx
	@rm -rf $(NAME_S) $(NAME_C)

clean:
	#@make clean -C libmx
	@rm -rf $(INC)
	@rm -rf $(SRC) $(SRC_SERVER) $(SRC_CLIENT)
	@rm -rf $(OBJ) $(OBJ_SERVER) $(OBJ_CLIENT)
	@rm -rf ./obj

reinstall: uninstall install

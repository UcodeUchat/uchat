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

LMXD =	libmx
LBMX = libmx.a
LMXA:=	$(addprefix $(LMXD)/, $(LBMX))
LMXI:=	$(LMXD)/inc
LBMXD = libmx
LIBMX = libmx

LJSOND	= libjson
LJSONX  = libjsonc.a
LJSONA := $(addprefix $(LJSOND)/, $(LJSONX))

LIBSNDFD  = libsndfile
LIBSNDFX  = libsndfile.a
LIBSNDFA := $(addprefix $(LIBSNDFD)/, $(LIBSNDFX))

LIBPORTAUDIOD  = libportaudio
LIBPORTAUDIOX  = lib/.libs/libportaudio.a
LIBPORTAUDIOA := $(addprefix $(LIBPORTAUDIOD)/, $(LIBPORTAUDIOX))

INCS = inc/uchat.h

SRC_SERVER = main_server.c \
	start_server.c \
	start_server2.c \
	set_daemon.c \
	server_worker.c \
	work_with_db.c \
	request_for_bd.c \
	run_function_type.c \
	process_message_in_server.c \
	save_file_in_server.c \
	send_file_from_server.c \
	send_file_from_server2.c \
	send_file_from_server3.c \
	authorization.c \
	authorization2.c \
	work_with_socket_list.c \
	work_with_socket_list_2.c \
	work_with_files_list.c \
	request_for_rooms.c \
	list_room.c \
	logout.c \
	functions_for_server.c \
	send_mail_notification.c \
	send_mail_notification2.c \
	send_mail_notification3.c \
	send_mail_notification4.c

SRC_CLIENT = main_client.c \
	start_client.c \
	send_message.c \
	save_file_in_client.c \
	send_file_from_client.c \
	input_from_server.c \
	gui.c \
	login.c \
	reg.c \
	general.c \
	general_functions.c \
	menu.c \
	menu_functions.c \
	create_room_menu.c \
	create_room.c \
	create_room_functions1.c \
	create_room_functions2.c \
	init_search.c \
	init_stickers.c \
	load_profile.c \
	load_profile_functions.c \
	load_profile_functions1.c \
	load_profile_functions2.c \
	message.c \
	search.c \
	search_functions.c \
	search_functions1.c \
	search_functions2.c \
	callbacks1.c \
	callbacks2.c \
	callbacks3.c \
	callbacks4.c \
	callbacks5.c \
	callbacks6.c \
	callbacks7.c \
	callbacks8.c \
	callbacks9.c \
	callbacks10.c \
	work_with_files_list_in_client.c \
	record_audio.c \
	play_audio.c \
	send_empty_json.c \

# SRC_HELP = $(wildcard *.c)
SRC_HELP = err_exit.c \
	functions.c \
	crypto.c \
	json_functions.c

SRCS_SERVER = $(addprefix $(SRCD)/server/, $(SRC_SERVER))
SRCS_CLIENT = $(addprefix $(SRCD)/client/, $(SRC_CLIENT))
SRCS_HELP = $(addprefix $(SRCD)/functions/, $(SRC_HELP))

OBJS_SERVER = $(addprefix $(OBJD)/, $(SRC_SERVER:%.c=%.o))
OBJS_CLIENT = $(addprefix $(OBJD)/, $(SRC_CLIENT:%.c=%.o))
OBJS_HELP = $(addprefix $(OBJD)/, $(SRC_HELP:%.c=%.o))

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -g -fsanitize=address

LIBRESSL_A = ./libressl_3/tls/.libs/libtls.a \
			 ./libressl_3/ssl/.libs/libssl.a \
			 ./libressl_3/crypto/.libs/libcrypto.a

LIBRESSL_H = \
			-I ./libressl_3/include/tls.h \
			-I ./libressl_3/include/openssl \
			-I ./libressl_3/include/pqueue.h \
			-I ./libressl_3/tls \
			-I ./libressl_3/ssl \
			-I ./libressl_3/crypto

#AUDIOFLAGS = -lportaudio
#TLSFLAGS =  -lcrypto -lssl -ltls
SQLFLAGS = -lsqlite3

all: install

server: $(NAME_S) $(LJSONX) $(LIBSNDFX) $(LIBPORTAUDIOX) $(LIBMX)

$(NAME_S): $(OBJS_SERVER) $(OBJS_HELP)

	@make -sC $(LJSOND)
	@clang $(CFLAGS) `pkg-config --cflags --libs gtk+-3.0` $(LMXA) $(LJSONA) $(LIBRESSL_H) $(LIBRESSL_A) $(OBJS_SERVER) $(OBJS_HELP) -o $@  $(SQLFLAGS)
	@printf "\r\33[2K$@\t   \033[32;1mcreated\033[0m\n"

$(OBJD)/%.o: src/server/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
	@printf "\r\33[2K\033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

$(OBJD)/%.o: src/functions/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
	@printf "\r\33[2K\033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

$(OBJS_SERVER): | $(OBJD)
$(OBJS_HELP): | $(OBJD)

$(OBJD):
	@mkdir -pv $@

$(LMXA):
	@make -sC $(LBMXD)

$(LIBMX): $(LMXA)
	@make -sC $(LBMXD)

$(LJSONA):
	@make -sC $(LJSOND)

$(LJSONX): $(LJSONA)
	@make -sC $(LJSOND)

$(LIBSNDFA):
	@make -sC $(LIBSNDFD)

$(LIBSNDFX): $(LIBSNDFA)
	@make -sC $(LIBSNDFD)

$(LIBPORTAUDIOA):
	(cd ./$(LIBPORTAUDIOD) &&./configure --disable-mac-universal)
	@make -sC $(LIBPORTAUDIOD)

$(LIBPORTAUDIOX): $(LIBPORTAUDIOA)
	@make -sC $(LIBPORTAUDIOD)


#$(LIBRESSLD_TLSA):
	#(cd ./$(LIBRESSLD) && ./configure BUILD_SHARED_LIBS=ON)
#	@make -sC $(LIBRESSLD)

#$(LIBRESSL_TLSX): $(LIBRESSLD_TLSA)
#	@make -sC $(LIBRESSLD)

client: $(NAME_C) #$(LIBSNDFX) $(LIBPORTAUDIOX) #$(LIBMX)


$(NAME_C): $(OBJS_CLIENT) $(OBJS_HELP)
	@clang $(CFLAGS) `pkg-config --cflags --libs gtk+-3.0` $(LMXA)  $(LJSONA) $(LIBSNDFA) -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon $(LIBPORTAUDIOA) $(LIBRESSL_H) $(LIBRESSL_A) $(OBJS_CLIENT) $(OBJS_HELP) -o $@
	@printf "\r\33[2K$@\t\t   \033[32;1mcreated\033[0m\n"


$(OBJD)/%.o: src/client/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
	@printf "\r\33[2K\033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "


$(OBJD)/%.o: src/functions/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
	@printf "\r\33[2K\033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

$(OBJS_CLIENT): | $(OBJD)

install: server client

clean:
# 	@make -sC ./libjson clean
# 	@make -sC ./libsndfile clean
# 	@make -sC $(LBMXD) clean
	@rm -rf $(OBJD)
	@printf "$(OBJD)\t\t   \033[31;1mdeleted\033[0m\n"

uninstall: clean
# 	@make -sC ./libjson uninstall
# 	@make -sC ./libsndfile clean
# 	@make -sC $(LBMXD) uninstall
	@rm -rf $(NAME_S) $(NAME_C)
	@printf "$(NAME_S)\t   \033[31;1muninstalled\033[0m\n"
	@printf "$(NAME_C)\t\t   \033[31;1muninstalled\033[0m\n"

reinstall: uninstall install
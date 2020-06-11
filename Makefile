NAME_S = uchat_server
NAME_C = uchat

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
LIBPORTAUDIO_FLAGS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit \
                     -framework CoreServices -framework Carbon

INCS = inc/uchat.h

SRC_SERVER = main_server.c \
	start_server.c \
	start_server2.c \
	start_server3.c \
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
	list_room2.c \
	logout.c \
	logout2.c \
	logout3.c \
	logout4.c \
	logout5.c \
	logout6.c \
	functions_for_server.c \
	send_mail_notification.c \
	send_mail_notification2.c \
	send_mail_notification3.c \
	send_mail_notification4.c \
	reconnection.c \
	validation.c \
	check_json.c \
	check_json2.c \
	check_json3.c \
	check_json4.c \
	email_notify.c \
	delete.c

SRC_CLIENT = main_client.c \
	start_client.c \
	start_client2.c \
	send_message.c \
	save_file_in_client.c \
	send_file_from_client.c \
	send_file_from_client2.c \
	input_from_server.c \
	run_functions_type.c \
	run_functions_type1.c \
	run_functions_type2.c \
	run_functions_type3.c \
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
	message_functions1.c \
	message_functions2.c \
	message_functions3.c \
	message_functions4.c \
	message_functions5.c \
	message_functions6.c \
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
	callbacks11.c \
	callbacks12.c \
	work_with_files_list_in_client.c \
	record_audio.c \
	record_audio2.c \
	play_audio.c \
	play_audio2.c \
	play_audio3.c \
	send_empty_json.c \
	mx_audio.c \

SRC_HELP = err_exit.c \
	functions.c \
	functions2.c \
	functions3.c \
	functions4.c \
	crypto.c \
	json_functions.c \
	json_short.c \
	json_short2.c \
	json_short3.c
	
BD = src/functions/create_bd.c
SRCS_SERVER = $(addprefix $(SRCD)/server/, $(SRC_SERVER))
SRCS_CLIENT = $(addprefix $(SRCD)/client/, $(SRC_CLIENT))
SRCS_HELP = $(addprefix $(SRCD)/functions/, $(SRC_HELP))

OBJS_SERVER = $(addprefix $(OBJD)/, $(SRC_SERVER:%.c=%.o))
OBJS_CLIENT = $(addprefix $(OBJD)/, $(SRC_CLIENT:%.c=%.o))
OBJS_HELP = $(addprefix $(OBJD)/, $(SRC_HELP:%.c=%.o))

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -g -fsanitize=address

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

SQLFLAGS = -lsqlite3

all: install

server: $(NAME_S) $(LJSONX) $(LIBSNDFX) $(LIBPORTAUDIOX) $(LIBMX)

$(NAME_S): $(OBJS_SERVER) $(OBJS_HELP)
	@clang -lsqlite3 -o create $(BD) && ./create server_db.bin
	@rm -rf create
	@make -sC $(LJSOND)
	@clang $(CFLAGS) `pkg-config --cflags --libs gtk+-3.0` $(LMXA) $(LJSONA) $(LIBRESSL_H) $(LIBRESSL_A) \
	       $(OBJS_SERVER) $(OBJS_HELP) -o $@  $(SQLFLAGS)
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
	@make -sC $(LIBPORTAUDIOD)

$(LIBPORTAUDIOX): $(LIBPORTAUDIOA)
	@make -sC $(LIBPORTAUDIOD)

client: $(NAME_C) $(LIBSNDFX) $(LIBPORTAUDIOX) $(LIBMX)

$(NAME_C): $(OBJS_CLIENT) $(OBJS_HELP)
	@clang $(CFLAGS) `pkg-config --cflags --libs gtk+-3.0` $(LMXA) $(LJSONA) $(LIBSNDFA) $(LIBPORTAUDIO_FLAGS) \
	       $(LIBPORTAUDIOA) $(LIBRESSL_H) $(LIBRESSL_A) $(OBJS_CLIENT) $(OBJS_HELP) -o $@
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
	@rm -rf $(OBJD)
	@printf "$(OBJD)\t\t   \033[31;1mdeleted\033[0m\n"

uninstall: clean
	@rm -rf $(NAME_S) $(NAME_C)
	@printf "$(NAME_S)\t   \033[31;1muninstalled\033[0m\n"
	@printf "$(NAME_C)\t\t   \033[31;1muninstalled\033[0m\n"

reinstall: uninstall install
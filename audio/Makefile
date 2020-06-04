NAME_PLAY = play
NAME_REC =rec

FLAGS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon
FLAGS_A =  ../libportaudio/lib/.libs/libportaudio.a \
                       ../libsndfile/libsndfile.a \


all: install

rec: $(NAME_REC)

$(NAME_REC):
	@clang paex_record_file.c -o rec_3 $(FLAGS) $(FLAGS_A)

install: rec

# -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon ../libportaudio/lib/.libs/libportaudio.a ../libsndfile/libsndfile.a
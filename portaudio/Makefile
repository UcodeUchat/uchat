NAME = portaudio.a

SRCS = \
	src/common/pa_allocation.c \
    src/common/pa_converters.c \
    src/common/pa_cpuload.c \
    src/common/pa_debugprint.c \
    src/common/pa_dither.c \
    src/common/pa_front.c \
    src/common/pa_process.c \
    src/common/pa_ringbuffer.c \
    src/common/pa_stream.c \
    src/common/pa_trace.c \
	\
	src/hostapi/coreaudio/pa_mac_core.c \
	src/hostapi/coreaudio/pa_mac_core_blocking.c \
	src/hostapi/coreaudio/pa_mac_core_utilities.c \
	\
	src/os/unix/pa_unix_util.c \
	src/os/unix/pa_unix_hostapis.c \
#	src/hostapi/coreaudio/pa_mac_core_old.c \
    \
#	src/hostapi/skeleton/pa_hostapi_skeleton.c \
	\
	\

#	src/pablio/pablio.c
#    src/hostapi/oss/pa_unix_oss.c \
#    src/hostapi/oss/recplay.c \
#    src/hostapi/jack/pa_jack.c \
#	src/hostapi/dsound/pa_win_ds.c \
#    src/hostapi/dsound/pa_win_ds_dynlink.c \

INC = inc/portaudio.h

OBJ_DIR = obj
OBJS = $(SRCS:.c=.o)
CFLAGS = -std=c11 -Wall

all: install

install: $(NAME)

$(NAME): $(OBJS)
	@ar -rcs $(NAME) $(OBJS)
	@mkdir -p $(OBJ_DIR)
	@mv $(OBJS) $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRCS)
	@clang $(CFLAGS) -o $@ -c $< $(INC)

uninstall:
	@rm -rf $(NAME)

clean:
	@rm -rf ./obj

reinstall: uninstall install

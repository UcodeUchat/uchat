NAME = portaudio.a

SRCS = \
	src/common/pa_allocation.c \
    src/common/pa_converters.c \
    src/common/pa_cpuload.c \
    src/common/pa_dither.c \
    src/common/pa_debugprint.c \
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
	\
	src/hostapi/skeleton/pa_hostapi_skeleton.c \
	\
	qa/loopback/src/audio_analyzer.c \
	qa/loopback/src/biquad_filter.c \
	qa/loopback/src/paqa_tools.c \
	qa/loopback/src/test_audio_analyzer.c \
	qa/loopback/src/write_wav.c \
	qa/loopback/src/paqa.c
#	src/hostapi/coreaudio/pa_mac_core_old.c \
    \
	\
	\

#	src/pablio/pablio.c
#    src/hostapi/oss/pa_unix_oss.c \
#    src/hostapi/oss/recplay.c \
#    src/hostapi/jack/pa_jack.c \
#	src/hostapi/dsound/pa_win_ds.c \
#    src/hostapi/dsound/pa_win_ds_dynlink.c \

top_srcdir = .

INC = inc/portaudio.h

OBJ_DIR = obj
OBJS = $(SRCS:.c=.o)

OTHER_OBJS = src/os/unix/pa_unix_hostapis.o \
 		src/os/unix/pa_unix_util.o \
 		src/hostapi/coreaudio/pa_mac_core.o \
 		src/hostapi/coreaudio/pa_mac_core_utilities.o \
 		src/hostapi/coreaudio/pa_mac_core_blocking.o \
 		src/common/pa_ringbuffer.o

CFLAGS = -std=c99 -g -O2 -DPA_LITTLE_ENDIAN -I$(top_srcdir)/include -I$(top_srcdir)/src/common -I$(top_srcdir)/src/os/unix -Wno-deprecated -Werror     -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DSIZEOF_SHORT=2 -DSIZEOF_INT=4 -DSIZEOF_LONG=8 -DHAVE_CLOCK_GETTIME=1 -DHAVE_NANOSLEEP=1 -DPA_USE_COREAUDIO=1
#CFLAGS = -std=c11 -Wall

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

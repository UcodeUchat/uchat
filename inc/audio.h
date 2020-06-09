#ifndef UCHAT_AUDIO_H
#define UCHAT_AUDIO_H

#include <stdbool.h>

#include "../libportaudio/include/portaudio.h"
#include "../libsndfile/src/sndfile.h"

#define MIN_TALKING_BUFFERS 8
#define TALKING_THRESHOLD_WEIGHT 0.99
#define TALKING_TRIGGER_RATIO 4.0
#define SAMPLE_RATE       (44100)  // в 1 секунде записи содержится 44100 семплов.
#define FRAMES_PER_BUFFER   (1024)
#define SAMPLE_SILENCE  (0.0f)
#define NUM_SECONDS     (5)
#define NUM_CHANNELS (2)

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif


#define BUFFER_LEN     512

typedef struct s_audio {
    uint16_t format_type;
    uint8_t number_channels;
    uint32_t sample_rate;
    size_t size;
    char *file_name;
    float *rec_samples;
}               t_audio;

typedef struct s_a_snippet {
    float *snippet;
    size_t size;
}           t_a_snippet;


//int mx_record_audio(void);
char *mx_record_audio(void);
int mx_init_stream(PaStream **stream, t_audio *data, t_a_snippet *sample_block);
int mx_exit_stream(PaStream **stream, t_audio *data, PaError err);
//int mx_exit_stream(t_audio *data, PaError err);
long mx_save_audio(t_audio *data);
int mx_process_stream_ext(PaStream *stream, t_audio *data,
                          t_a_snippet *sample_block, const char *fileName,
                          bool *sample_complete);
float mx_rms(float *data, size_t len);
float mx_change_threshold(float talking_threshold, float talking_ntensity);
int mx_play_sound_file(char *file_name, char *start_time, char *duration_t);
int mx_play_sound_file2(char *file_name, char *start_time, char *duration_t);

#endif //UCHAT_AUDIO_H

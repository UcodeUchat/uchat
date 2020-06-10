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
#define NUM_SECONDS     (15)


#define BUFFER_LEN      1024


typedef struct s_a_file {
    sf_count_t length;  // length of file in frames
    sf_count_t start_point;  // start_point of frames read
    sf_count_t end_point;  // end point of frames playing
}               t_a_file;

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


//int mx_record_audio(void)
int mx_init_stream(PaStream **stream, t_audio *data, t_a_snippet *sample_block);
int mx_exit_stream(t_audio *data, PaError err);
long mx_save_audio(t_audio *data);
void mx_print_s_info(SF_INFO s_info);
int mx_exit_player(PaError err, const char *text, SNDFILE* a_file);


#endif //UCHAT_AUDIO_H

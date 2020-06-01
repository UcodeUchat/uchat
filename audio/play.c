#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <math.h>
#include "../libportaudio/include/portaudio.h"
#include "../libsndfile/src/sndfile.h"
#include <math.h>

#define MIN_TALKING_BUFFERS 8
#define TALKING_THRESHOLD_WEIGHT 0.99
#define TALKING_TRIGGER_RATIO 4.0
#define SAMPLE_RATE       (44100)  // в 1 секунде записи содержится 44100 семплов.
#define FRAMES_PER_BUFFER   (1024)
#define SAMPLE_SILENCE  (0.0f)
#define NUM_SECONDS     (4)
#define BUFFER_LEN      1024


static int init_stream(PaStream** stream, SF_INFO s_info) {
    PaStreamParameters output_parameters;
    PaError err;

    output_parameters.device = Pa_GetDefaultOutputDevice();
    output_parameters.channelCount = s_info.channels;
    output_parameters.sampleFormat = paFloat32;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(stream, NULL, &output_parameters, s_info.samplerate,
            paFramesPerBufferUnspecified,
            paClipOff,
            NULL,
            NULL);
    if (err != paNoError || ! stream) {
        printf("error init_stream =%s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return -1;
    }
    return Pa_StartStream(*stream);
}

int mx_play_sound_file(char *file_name) {
    PaStream* stream = NULL;
    SNDFILE* a_file = NULL;
    SF_INFO s_info;
    PaError err;

//    printf("port audio %d\n",  Pa_GetVersion());
//    printf("port audio %s\n",  Pa_GetVersionText());
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("error Pa_Initialize =%s\n", Pa_GetErrorText(err));
        return -1;
    }
    memset(&s_info, 0, sizeof(s_info));
    a_file = sf_open(file_name, SFM_READ, &s_info);
    if (!a_file) {
        printf("error open =%d\n", sf_error(a_file));
        printf("error str open =%s\n", sf_error_number(a_file));
        Pa_Terminate();
        return -1;
    }
    if (s_info.channels > 1) {
        return -1;
    }
    err = init_stream(&stream, s_info);
    if (err){
        fprintf(stderr, "%s\n", "error");
        return err;
    }

    sf_count_t read_count = 0;
    float data[BUFFER_LEN];
    memset(data, 0, sizeof(data));
    int subFormat = s_info.format & SF_FORMAT_SUBMASK;
    double scale = 1.0;
    int m = 0;
    while ((read_count = sf_read_float(a_file, data, BUFFER_LEN))) {
        if (subFormat == SF_FORMAT_FLOAT || subFormat == SF_FORMAT_DOUBLE) {
            for (m = 0 ; m < read_count ; ++m) {
                data[m] *= scale;
            }
        }
        err = Pa_WriteStream(stream, data, BUFFER_LEN);
        if (err != paNoError) {
            printf("error Pa_WriteStream =%s\n", Pa_GetErrorText(err));
            break;
        }
        memset(data, 0, sizeof(data));
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        printf("error Pa_CloseStream =%s\n", Pa_GetErrorText(err));
    Pa_Terminate();
    sf_close(a_file);
    return 0;
}

int main (int argc, char * argv []) {
    mx_play_sound_file(argv[1]);
    return 0;

}

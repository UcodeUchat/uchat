#include "uchat.h"

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
        printf("error sf_info =%s\n", Pa_GetErrorText(err));
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

    printf("port audio %d\n",  Pa_GetVersion());
    printf("port audio %s\n",  Pa_GetVersionText());
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("error sf_info =%s\n", Pa_GetErrorText(err));
        return -1;
    }
    memset(&s_info, 0, sizeof(s_info));
    a_file = sf_open(file_name, SFM_READ, &s_info);
    if (!a_file) {
        printf("error sf_info =%d\n", sf_error(a_file));
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
    while ((read_count = sf_read_float(a_file, data, BUFFER_LEN))) {
//        if (subFormat == SF_FORMAT_FLOAT || subFormat == SF_FORMAT_DOUBLE) {
//            int m = 0;
//            for (m = 0 ; m < read_count ; ++m) {
//                data[m] *= scale;
//            }
//        }
        err = Pa_WriteStream(stream, data, BUFFER_LEN);
        if (err != paNoError) {
            printf("error Pa_WriteStream =%s\n", Pa_GetErrorText(err));
            break;
        }
        memset(data, 0, sizeof(data));
    }
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        printf("error Pa_WriteStream =%s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return false;
    }
    Pa_Terminate();
    return 0;
}

// Created by snikolayen on 14.05.2020.
//


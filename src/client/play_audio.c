#include "uchat.h"

static int init_stream(PaStream** stream, SF_INFO s_info) {
    PaStreamParameters output_parameters;
    PaError err;

    output_parameters.device = Pa_GetDefaultOutputDevice();
    output_parameters.channelCount = s_info.channels;
    output_parameters.sampleFormat = paFloat32;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo\
    (output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(stream, NULL, &output_parameters, s_info.samplerate,
                        paFramesPerBufferUnspecified, paClipOff, NULL, NULL);
    if (err != paNoError || ! stream)
        return mx_exit_player(err, "Pa_OpenStream", NULL);
    return Pa_StartStream(*stream);
}

static void init_a_file(t_a_file *audio, SF_INFO s_info,
                        char *start_t, char *duration_t) {
    double dur = 0;

    audio->length = s_info.frames;
    if (start_t)
        audio->start_point = (sf_count_t)(atof(start_t)) * s_info.samplerate;
    else
        audio->start_point = 0;  // start play with the beginning
    if (duration_t) {
        dur = atof(duration_t);
        audio->end_point = (sf_count_t)(dur * s_info.samplerate *
                                        s_info.channels + audio->start_point);
        audio->end_point = (audio->end_point < audio->length) ? \
                            audio->end_point : audio->length;
    }
    else {
        audio->end_point = audio->length;
        dur = (double)(audio->end_point - audio->start_point) / \
               (double)s_info.samplerate;
    }
    audio->read_count = 0;
}

char *mx_get_file_name(t_mes *mes) {
    char *file_name = NULL;

    if (mes != NULL) {
        file_name = mx_strjoin("./Uchat_downloads/", mes->message->data);
        printf("file_name = %s\n", file_name);
    }
    else
        file_name = mx_strdup("audio/message_receive.aiff");
    return file_name;
}

//
//SNDFILE *mx_init_struct_play(SF_INFO *s_info, PaError *err){
//    PaError err;
//
//
//}


static int play_loop(t_mes *mes, SNDFILE* a_file, SF_INFO s_info,
                     PaStream* stream) {
    sf_count_t read_count = 0;
    float data[BUFFER_LEN * s_info.channels];
    double scale = 1.0;
    int m = 0;
    int subFormat = s_info.format & SF_FORMAT_SUBMASK;
    PaError err;

    memset(data, 0, sizeof(data));
    while ((read_count = sf_read_float(a_file, data, BUFFER_LEN))
            && mes->audio->pause == 0) {
        if (subFormat == SF_FORMAT_FLOAT || subFormat == SF_FORMAT_DOUBLE) {
            for (m = 0 ; m < read_count ; ++m)
                data[m] *= scale;
        }
        err = Pa_WriteStream(stream, data, BUFFER_LEN / s_info.channels);
        if (err != paNoError) {
            printf("error Pa_WriteStream =%s\n", Pa_GetErrorText(err));
            return 1;
        }
        memset(data, 0, sizeof(data));
    }
    return 0;
}


int mx_play_sound_file(t_mes *mes, char *start_time, char *duration_t) {
    PaStream* stream = NULL;
    SNDFILE* audio_file = NULL;
    SF_INFO s_info;
    PaError err;
    t_a_file *audio = malloc(sizeof(t_a_file));

    audio->file_name = strdup(mx_get_file_name(mes));
    if ((err = Pa_Initialize()) != paNoError)
        return mx_exit_player(err, "Pa_Initialize", audio_file);
    memset(&s_info, 0, sizeof(s_info));
    if (!(audio_file = sf_open(audio->file_name, SFM_READ, &s_info))) {
        printf("error sf_info = %d\n", sf_error(audio_file));
        Pa_Terminate();
        return 1;
    }
    mx_print_s_info(s_info);
    init_a_file(audio, s_info, start_time, duration_t);
    err = init_stream(&stream, s_info);
    if (err != paNoError)
        return mx_exit_player(err, "init_stream", audio_file);
    sf_seek(audio_file, audio->start_point, SEEK_SET);
    play_loop(mes, audio_file, s_info, stream);
    if ((err = Pa_CloseStream(stream)) != paNoError)
        printf("error Pa_CloseStream =%s\n", Pa_GetErrorText(err));
    Pa_Terminate();
    sf_close(audio_file);
    return 0;
}


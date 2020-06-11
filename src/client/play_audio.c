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
    audio->read_sum = 0;
}

static int mx_play_init(t_a_file *audio, SF_INFO *s_info) {
    PaError err;

    if ((err = Pa_Initialize()) != paNoError)
        return mx_exit_player(err, "Pa_Initialize", audio->audio_file);
    if (!(audio->audio_file = sf_open(audio->file_name, SFM_READ, s_info))) {
        printf("error sf_info = %d\n", sf_error(audio->audio_file));
        Pa_Terminate();
        return 1;
    }
    return 0;
}

int mx_play_sound_file(t_mes *mes, char *start_time, char *duration_t) {
    PaStream* stream = NULL;
    SF_INFO s_info;
    PaError err;
    t_a_file *audio = malloc(sizeof(t_a_file));

    audio->file_name = strdup(mx_get_file_name(mes));
    memset(&s_info, 0, sizeof(s_info));
    if ((err = mx_play_init(audio, &s_info)))
        return err;
    init_a_file(audio, s_info, start_time, duration_t);
    err = init_stream(&stream, s_info);
    if (err != paNoError)
        return mx_exit_player(err, "init_stream", audio->audio_file);
    sf_seek(audio->audio_file, audio->start_point, SEEK_SET);
    mx_play_loop(mes, audio, s_info, stream);
    if ((err = Pa_CloseStream(stream)) != paNoError)
        printf("error Pa_CloseStream =%s\n", Pa_GetErrorText(err));
    Pa_Terminate();
    sf_close(audio->audio_file);
    return 0;
}


int mx_play_audio_notific(char *file, char *start_time, char *duration_t) {
    PaStream* stream = NULL;
    SF_INFO s_info;
    PaError err;
    t_a_file *audio = malloc(sizeof(t_a_file));

    audio->file_name = strdup(file);
    memset(&s_info, 0, sizeof(s_info));
    if ((err = mx_play_init(audio, &s_info)))
        return err;
    init_a_file(audio, s_info, start_time, duration_t);
    err = init_stream(&stream, s_info);
    if (err != paNoError)
        return mx_exit_player(err, "init_stream", audio->audio_file);
    sf_seek(audio->audio_file, audio->start_point, SEEK_SET);
    mx_play_loop_simple(audio, s_info, stream);
    if ((err = Pa_CloseStream(stream)) != paNoError)
        printf("error Pa_CloseStream =%s\n", Pa_GetErrorText(err));
    Pa_Terminate();
    sf_close(audio->audio_file);
    return 0;
}


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
                        paFramesPerBufferUnspecified, paClipOff, NULL, NULL);
    if (err != paNoError || ! stream)
        return mx_exit_player(err, "Pa_OpenStream", NULL);
    return Pa_StartStream(*stream);
}

static void init_a_file(t_a_file *audio, SF_INFO s_info,
                        char *start_time, char *duration_t) {
    double starttime = 0;
    double duration = 0;

    audio->length = s_info.frames;
    if (start_time) {
        starttime = atof(start_time);
        audio->start_point = (sf_count_t) starttime * s_info.samplerate;
    }
    else
        audio->start_point = 0;  // start play with the beginning
    if (duration_t) {
        duration = atof(duration_t);
        audio->end_point = (sf_count_t) (duration * s_info.samplerate * s_info.channels + audio->start_point);
        audio->end_point = (audio->end_point < audio->length) ? audio->end_point : audio->length;
    }
    else {
        audio->end_point = audio->length;
        duration = (double) (audio->end_point - audio->start_point) / (double) s_info.samplerate;
    }
    printf("length frames =%lld\n", audio->length);
    printf("starttime  =%f\n", starttime);
    printf("duration  =%f\n", duration);
    printf("start_point frames =%lld\n", audio->start_point);
    printf("end_point frames =%lld\n\n", audio->end_point);
}




int mx_play_sound_file(t_mes *mes, char *start_time, char *duration_t) {
    PaStream* stream = NULL;
    SNDFILE* a_file = NULL;
    SF_INFO s_info;
    PaError err;
    t_a_file *audio = malloc(sizeof(t_a_file));

    sf_count_t read_count = 0;
    sf_count_t read_sum = 0;
    char *file_name = NULL;

    if (mes != NULL) {
        file_name = mx_strjoin("./Uchat_downloads/", mes->message->data);
        printf("file_name = %s\n", file_name);
    }
    else
        file_name = mx_strdup("audio/message_receive.aiff");

    if ((err = Pa_Initialize()) != paNoError)
        return mx_exit_player(err, "Pa_Initialize", a_file);

    memset(&s_info, 0, sizeof(s_info));
    if (!(a_file = sf_open(file_name, SFM_READ, &s_info))) {
        printf("error sf_info = %d\n", sf_error(a_file));
        Pa_Terminate();
        return 1;
    }

    mx_print_s_info(s_info);
    printf("3\n");

    init_a_file(audio, s_info, start_time, duration_t);
    printf("4\n");

    err = init_stream(&stream, s_info);
    if (err != paNoError)
        return mx_exit_player(err, "init_stream", a_file);

    printf("5\n");

    float data[BUFFER_LEN * s_info.channels];
    memset(data, 0, sizeof(data));
    int subFormat = s_info.format & SF_FORMAT_SUBMASK;
    double scale = 1.0;
    int m = 0;

    sf_seek(a_file, audio->start_point, SEEK_SET);

    while ((read_count = sf_read_float(a_file, data, BUFFER_LEN)) && mes->audio->pause == 0) {
        if (subFormat == SF_FORMAT_FLOAT || subFormat == SF_FORMAT_DOUBLE) {
            for (m = 0 ; m < read_count ; ++m) {
                data[m] *= scale;
            }
        }
        read_sum += read_count;
//        if (s_info.channels == 2)
//            read_sum += read_count;
//         if (read_sum > end_point - start_point) {
//             printf("read_sum frames =%lld\n", read_sum);
//             break;
//         }
        err = Pa_WriteStream(stream, data, BUFFER_LEN / s_info.channels);
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

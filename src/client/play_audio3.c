#include "uchat.h"

void mx_check_sub_format(SF_INFO s_info, float *data, sf_count_t read_count) {
    int m = 0;
    double scale = 1.0;
    int sub_format = s_info.format & SF_FORMAT_SUBMASK;

    if (sub_format == SF_FORMAT_FLOAT || sub_format == SF_FORMAT_DOUBLE) {
        for (m = 0 ; m < read_count ; ++m)
            data[m] *= scale;
    }
}

int mx_play_loop(t_mes *mes, t_a_file *audio, SF_INFO s_info,
                 PaStream* stream) {
    sf_count_t read_count = 0;
    float data[BUF_LEN * s_info.channels];
    PaError err;

    memset(data, 0, sizeof(data));
    while ((read_count = sf_read_float(audio->audio_file, data, BUF_LEN))
           && mes->audio->pause == 0) {
        mx_check_sub_format(s_info, data, read_count);
        audio->read_sum += read_count;
        if (s_info.channels == 1 && (audio->read_sum > audio->end_point -
                                                       audio->start_point))
            break;
        err = Pa_WriteStream(stream, data, BUF_LEN / s_info.channels);
        if (err != paNoError) {
            mx_err_return2("error Pa_WriteStream=%s\n", Pa_GetErrorText(err));
        }
        memset(data, 0, sizeof(data));
    }
    return 0;
}

int mx_play_loop_simple(t_a_file *audio, SF_INFO s_info,
                 PaStream* stream) {
    sf_count_t read_count = 0;
    float data[BUF_LEN * s_info.channels];
    PaError err;

    memset(data, 0, sizeof(data));
    while ((read_count = sf_read_float(audio->audio_file, data, BUF_LEN))) {
        mx_check_sub_format(s_info, data, read_count);
        audio->read_sum += read_count;
        if (s_info.channels == 1 && (audio->read_sum > audio->end_point -
                                     audio->start_point))
            break;
        err = Pa_WriteStream(stream, data, BUF_LEN / s_info.channels);
        if (err != paNoError) {
            mx_err_return2("error Pa_WriteStream=%s\n", Pa_GetErrorText(err));
        }
        memset(data, 0, sizeof(data));
    }
    return 0;
}

char *mx_get_file_name(t_mes *mes) {
    char *file_name = NULL;

    if (mes != NULL) {
        file_name = mx_strjoin("./Uchat_downloads/", mes->message->data);
        printf("file_name = %s\n", file_name);
    }
    return file_name;
}

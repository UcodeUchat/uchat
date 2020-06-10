#include "uchat.h"

int mx_exit_stream(t_audio *data, PaError err) {
    Pa_Terminate();
    if (data->rec_samples)
        free (data->rec_samples);
    if (err != paNoError) {
        fprintf(stderr, "Error occured while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText( err ));
        err = 1; // Always return 0 or 1, but no other return codes.
    }
    return err;
}

long mx_save_audio(t_audio *data) {
    uint8_t err = SF_ERR_NO_ERROR;
    SF_INFO sfinfo ={
            .channels = data->number_channels,
            .samplerate = data->sample_rate,
            .format = SF_FORMAT_AIFF | SF_FORMAT_FLOAT
    };
    char file_name[100];

    snprintf(file_name, 100, "./Uchat_downloads/rec_massage%d.aif", rand());
    SNDFILE *outfile = sf_open(file_name, SFM_WRITE, &sfinfo);
    if (!outfile) {
        printf("error outfile =%d\n", sf_error(outfile));
        return -1;
    }
    long wr = sf_writef_float(outfile, data->rec_samples, data->size / 8);
    err = data->size - wr;
    sf_write_sync(outfile);
    sf_close(outfile);
    data->file_name = strdup(file_name);
    return err;
}





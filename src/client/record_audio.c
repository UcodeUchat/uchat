#include "uchat.h"

static t_audio * init_audio_data() {
    t_audio *data = malloc(sizeof(t_audio));
    data->format_type = paFloat32;
    data->number_channels = 0;
    data->sample_rate = SAMPLE_RATE;
    data->size = 0;
    data->rec_samples = NULL;
    return data;
}

static int process_stream(PaStream *stream, t_audio *data,
                          t_a_snippet *sample_block, int *i) {
    if (!stream || !data || !sample_block)
        return -1;
    (*i)++;
    Pa_ReadStream(stream, sample_block->snippet, FRAMES_PER_BUFFER);
    data->rec_samples = realloc(data->rec_samples, sample_block->size * (*i));
    data->size = sample_block->size * (*i);
    if (data->rec_samples) {
        size_t next_ndex = ((*i)- 1) * sample_block->size;
        char *destination = (char*)data->rec_samples + next_ndex;
        memcpy(destination, sample_block->snippet, sample_block->size);
    }
    else{
        free(data->rec_samples);
        data->rec_samples = NULL;
        data->size = 0;
    }
    return 0;
}

static int record(PaStream *stream, t_audio *data, t_a_snippet *sample_block,
                  t_client_info *info) {
    int err = 0;
    int j = 0;
    int i;

    printf("Wire on. Will run %d seconds.\n", NUM_SECONDS);
    fflush(stdout);
    for (i = 0; i < (NUM_SECONDS * SAMPLE_RATE) / FRAMES_PER_BUFFER; ++i) {
        if (info->can_record == 1)
            break;
        err = process_stream(stream, data, sample_block, &j);
    }
    mx_save_audio(data);
    printf("Wire off.\n"); fflush(stdout);
    err = Pa_StopStream(stream);
    if (err != paNoError)
        mx_exit_stream(data, err);
    return err;
}

char *mx_record_audio(t_client_info *info) {
    t_audio *data = init_audio_data();
    t_a_snippet *sample_block = malloc(sizeof(t_a_snippet));
    PaError err = paNoError;

    sample_block->snippet = NULL;
    sample_block->size = 0;
    PaStream *stream = NULL;
    printf(" start record\n");
    err = mx_init_stream(&stream, data, sample_block);
    if (err){
        fprintf(stderr, "%s\n", "error");
        return NULL;
    }
    err = record(stream, data, sample_block, info);
    if (err != 0)
        printf("err =%d\n", err);
    printf(" exit record\n");
    printf("record to file->%s\n", data->file_name);
    info->can_record = 1;
    return data->file_name;
}



/*
static int number_channels() {


}

*/



int mx_init_stream(PaStream **stream, t_audio *data, t_a_snippet *sample_block) {
    PaError err;
    PaStreamParameters input_parameters;
    PaStreamParameters output_parameters;
    const PaDeviceInfo* inputInfo;
    const PaDeviceInfo* outputInfo;
    int numChannels;

    const PaDeviceInfo *info;
    if (!stream || !data || !sample_block)
        return -1;

    printf("patest_read_write_wire.c\n"); fflush(stdout);
    printf("sizeof(int) = %lu\n", sizeof(int)); fflush(stdout);
    printf("sizeof(long) = %lu\n", sizeof(long)); fflush(stdout);

    err = Pa_Initialize();
    if (err != paNoError)
        return mx_exit_stream(data, err);

    err = Pa_GetDeviceCount();

    info = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
    if (!info) {
        fprintf(stdout, "%s\n", "Unable to find info on default input device.");
        return -1;
    }

    input_parameters.device = Pa_GetDefaultInputDevice();
    printf( "Input device # %d.\n", input_parameters.device);
    inputInfo = Pa_GetDeviceInfo(input_parameters.device);
    printf( "    Name: %s\n", inputInfo->name );
    printf( "      LL: %g s\n", inputInfo->defaultLowInputLatency);
    printf( "      HL: %g s\n", inputInfo->defaultHighInputLatency);
    if (input_parameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        return -1;

    }
    output_parameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    printf( "Output device # %d.\n", output_parameters.device);
    outputInfo = Pa_GetDeviceInfo( output_parameters.device);
    printf( "   Name: %s\n", outputInfo->name );
    printf( "     LL: %g s\n", outputInfo->defaultLowOutputLatency);
    printf( "     HL: %g s\n", outputInfo->defaultHighOutputLatency);

    numChannels = inputInfo->maxInputChannels < outputInfo->maxOutputChannels
                  ? inputInfo->maxInputChannels : outputInfo->maxOutputChannels;
    printf( "Num channels = %d.\n", numChannels );


    data->number_channels = numChannels;
    input_parameters.channelCount = numChannels;
    input_parameters.sampleFormat = paFloat32;
    input_parameters.suggestedLatency = inputInfo->defaultLowInputLatency;
    input_parameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(stream, &input_parameters, NULL, data->sample_rate, FRAMES_PER_BUFFER, paClipOff, NULL, NULL);
    if (err)
        return mx_exit_stream(data, err);
    sample_block->size = FRAMES_PER_BUFFER * sizeof(float) * data->number_channels;  //number bytes
    sample_block->snippet = malloc(sample_block->size);

    if( sample_block->snippet == NULL ) {
        printf("Could not allocate record array.\n");
        return mx_exit_stream(data, err);
    }
    memset(sample_block->snippet, SAMPLE_SILENCE, sample_block->size);

    return Pa_StartStream(*stream);
}




#include "uchat.h"

void mx_print_s_info(SF_INFO s_info) {
    printf ("frames = %lld\n", s_info.frames);
    printf ("samplerate = %d\n", s_info.samplerate);
    printf ("channels = %d\n", s_info.channels);
    printf ("format = %d\n", s_info.format);
    printf ("sections = %d\n", s_info.sections);
    printf ("seekable = %d\n", s_info.seekable);

}

int mx_exit_player(PaError err, const char *text, SNDFILE* a_file) {
    printf("error in %s =%s\n", text, Pa_GetErrorText(err));
    Pa_Terminate();
    if (a_file)
        sf_close(a_file);
    return 1;
}




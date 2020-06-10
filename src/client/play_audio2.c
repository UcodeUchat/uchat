#include "uchat.h"

void mx_print_s_info(SF_INFO s_info) {
    printf ("frames = %lld\n", s_info.frames);
    printf ("samplerate = %d\n", s_info.samplerate);
    printf ("channels = %d\n", s_info.channels);
    printf ("format = %d\n", s_info.format);
    printf ("sections = %d\n", s_info.sections);
    printf ("seekable = %d\n", s_info.seekable);

}



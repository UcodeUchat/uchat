#include "uchat.h"

void mx_print_tid(const char *s) {
    pid_t       pid;
    pthread_t   tid;

    pid = getpid();
    tid = pthread_self();
    printf("%s pid %u tid %u (0x%x)\n", s,
           (unsigned int)pid, (unsigned int)tid, (unsigned int)tid);
}


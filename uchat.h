#ifndef UCHAT_H
#define UCHAT_H

#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <sys/socket.h>
#include <resolv.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


#include "./libressl/include/tls.h"
#include "./libressl/include/openssl/evp.h"
#include "./libressl/include/openssl/sha.h"
#include "./libressl/include/openssl/aes.h"
#include "./libressl/include/openssl/conf.h"

#include "libmx/inc/libmx.h"

void mx_printerr(const char *s);
int mx_set_demon(const char *log_file);
void *mx_worker(void *arg);
//int main2(int argc, char *argv[]);
void mx_worker_ssl(SSL* ssl);
int mx_tls_worker(struct tls *tls_accept);

void mx_report_tls(struct tls * tls_ctx, char * host);
void mx_report_tls_client(struct tls * tls_ctx, char * host);

#endif


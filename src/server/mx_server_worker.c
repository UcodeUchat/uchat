#include "uchat.h"

int mx_tls_worker(struct tls *tls_accept) {
    char buf[1024];
    int rc;

    while(1) {
        rc = tls_read(tls_accept, buf, sizeof(buf));	// get request
        if (rc > 0 ) {
            buf[rc] = 0;
            printf("Client msg: %s\n", buf);
            tls_write(tls_accept, buf, strlen(buf));	// send reply
        }
        if (rc == -1 ) {
            tls_close(tls_accept);
            tls_free(tls_accept);
        }
    }

}

void *mx_worker(void *arg) {
    int client_sock = *((int *)arg);
    ssize_t size;
    time_t now;

    printf("run worker\n");
    printf("client_sock = %d\n", client_sock);
    printf("New client created\n");

    pthread_t thread_id = pthread_self();
    pid_t pid = getpid();
    printf("pid %d, tid %d: new thread, client socket = %d\n",
           (int)pid, (int)thread_id, *((int *) arg));

    while(1) {
        int buffer = 0;
        if ((size = read(client_sock, &buffer, sizeof(buffer))) == -1) {
            printf("error1 = %s\n", strerror(errno));
            continue;
        }
        if (size == 0) {
            printf("Closed connection\n");
            break;
        }
        now = time(0);
        char * time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0';
        printf("%s\tReceived %d\n", time_str, buffer);
        buffer++;
        if (write(client_sock, &buffer, sizeof(buffer)) == -1)
            printf("error2 = %s\n", strerror(errno));
        else
            printf("Sent %d\n\n", buffer);
    }
    close(client_sock);
    return NULL;
}


void mx_worker_ssl(SSL* ssl) {  // Serve the connection -- threadable
    char buf[1024];
//    char reply[1024];
    int sd, rc;
//    const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";

    if (SSL_accept(ssl) < 0 )					/* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
        printf("SSL/TLS using %s\n", SSL_get_cipher(ssl));
//        mx_show_certs(ssl);							// get any certificates
    while(1) {
        rc = SSL_read(ssl, buf, sizeof(buf));	// get request
        if (rc > 0 ) {
            buf[rc] = 0;
            printf("Client msg: \"%s\"\n", buf);
//            sprintf(reply, HTMLecho, buf);			// construct reply
            SSL_write(ssl, buf, strlen(buf));	// send reply
        }
//        else
//            ERR_print_errors_fp(stderr);
    }
    sd = SSL_get_fd(ssl);							/* get socket connection */
    SSL_free(ssl);									/* release SSL state */
    close(sd);										/* close connection */
}
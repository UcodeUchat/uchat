#include "uchat.h"

void mx_sha_hash_password(char *password) {
    size_t data_len = strlen(password);
    printf("data_len =  %zu\n", data_len);
    printf ("password = %s\n", password);
//    SHA_CTX shactx;
//     unsigned char hash_pas[SHA_DIGEST_LENGTH];
//     unsigned char* testdata = (unsigned char *)malloc(data_len);
//     for (size_t i = 0; i < data_len; i++)
//         testdata[i] = (unsigned char)password;
//     SHA1(testdata, data_len, hash_pas);
// //    SHA1((unsigned char)password, data_len, hash_pas);
// /*
//     SHA1_Init(&shactx);
//     SHA1_Update(&shactx, testdata, DataLen);
//     SHA1_Final(hash_pas, &shactx);
// */
//     for (size_t i = 0; i < SHA_DIGEST_LENGTH; i++)
//         printf("%02x", hash_pas[i]);
//     putchar('\n');
//    return hash_pas;
}


void mx_show_certs(SSL* ssl) {
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}


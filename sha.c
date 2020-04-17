#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

int main(int argc, char *argv[]) {
    size_t data_len = strlen(argv[1]);
    printf("data_len =  %zu\n", data_len);
//    SHA_CTX shactx;
    unsigned char hash_pas[SHA_DIGEST_LENGTH];

    unsigned char* testdata = (unsigned char *)malloc(data_len);
    for (size_t i = 0; i < data_len; i++)
        testdata[i] = (unsigned char)argv[1];
    SHA1(testdata, data_len, hash_pas);
/*
    SHA1_Init(&shactx);
    SHA1_Update(&shactx, testdata, DataLen);
    SHA1_Final(hash_pas, &shactx);
*/
    for (size_t i = 0; i < SHA_DIGEST_LENGTH; i++)
        printf("%02x", hash_pas[i]);
    putchar('\n');
    return 0;
//    return hash_pas;
}


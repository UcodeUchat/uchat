#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>

//libreSSL
//#include "sa.h"

//typedef unsigned char byte;

int main(int argc, char *argv[]) {
    const int DataLen = 30;
    SHA_CTX shactx;
//    byte digest[SHA_DIGEST_LENGTH];
    unsigned char hash_str[SHA_DIGEST_LENGTH];
    int i;

//    byte* testdata = (byte *)malloc(DataLen);
    unsigned char* testdata = (unsigned char *)malloc(DataLen);
    for (i = 0; i < DataLen; i++)
        testdata[i] = argv[i] ? (unsigned char)argv[i] : 0;

    SHA1_Init(&shactx);
    SHA1_Update(&shactx, testdata, DataLen);
    SHA1_Final(hash_str, &shactx);

    for (i = 0; i < SHA_DIGEST_LENGTH; i++)
	    printf("%02x", hash_str[i]);

    putchar('\n');
    return 0;
}


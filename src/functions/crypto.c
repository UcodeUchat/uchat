#include "uchat.h"

char *mx_strhash(const char *to_hash) {
    // SHA256_CTX c;
    unsigned char *result = NULL;

    result = SHA256((const unsigned char *)to_hash, strlen(to_hash), result);
    return (char *)result;
}

char *mx_encrypt(char *str) { // this function places the encrypted message in input string
    if (str == NULL)
        return NULL;
    else if (str[0]) {
        AES_KEY aes_key;
        int size = strlen(str);
        unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

        AES_set_encrypt_key(key, 32 * 8, &aes_key);

        if (size % 16 != 0) {
            str = realloc(str, (size += size % 16));
            str[size - 1] = '\0';
        }

        for(int i = 0; i < size; i += 16) {
            AES_encrypt((unsigned char *)str + i, (unsigned char *)str + i, &aes_key);
        }
        return str;
    }
    return str;
}

char *mx_decrypt(char *str) { // this function places the decrypted message in input string
    if (str == NULL)
        return NULL;
    else if (str[0]) {
        AES_KEY aes_key;
        int size = strlen(str);
        unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

        AES_set_decrypt_key(key, 32 * 8, &aes_key);

        if (size % 16 != 0) {
            str = realloc(str, (size += size % 16));
            str[size - 1] = '\0';
        }

        for(int i = 0; i < size; i += 16) {
            AES_decrypt((unsigned char *)str + i, (unsigned char *)str + i, &aes_key);
        }
    }
    return str;
}

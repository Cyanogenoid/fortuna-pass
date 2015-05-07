#include "crypto.h"

#include <avr/eeprom.h>
#include <stdlib.h>

char eep_priv_key[PRIVATE_KEY_LENGTH_BYTE] EEMEM;
bigint_t modulus = {
    .length_W = 3,
    .info = 0,
    .wordv = (bigint_t[]) {MODULUS_RIGHT, MODULUS_MIDDLE, MODULUS_LEFT},
};
static rsa_privatekey_t priv_key;
static bigint_t priv_d;

void load_private_key(char* key) {
    // load key
    char* buffer = (char*) malloc(PRIVATE_KEY_LENGTH_BYTE);
    eeprom_read_block(buffer, eep_priv_key, PRIVATE_KEY_LENGTH_BYTE);
    // decrypt with AES
    aes256_ctx_t context = {{0}};
    aes256_init(key, &context);
    size_t i;
    for (i = 0; i < PRIVATE_KEY_LENGTH_BYTE; i += 16) {
        aes256_dec(buffer+i, &context);
    }
    // make into bigint
    bigint_t local_priv_d = {
        .length_W = PRIVATE_KEY_LENGTH_BYTE,
        .wordv = buffer,
    };
    priv_d = local_priv_d;
    bigint_adjust(&priv_d);
    bigint_adjust(&modulus);
    // put into private key
    rsa_privatekey_t local_priv_key = {
        .n = 1,
        .modulus = modulus,
        .components = &priv_d,
    };
    priv_key = local_priv_key;
}

void decrypt(void* buffer, size_t length_B) {
    // bigint_t data = {
    //     .length_W = length_B,
    //     .wordv = buffer,
    // };
    // bigint_adjust(&data);
    // rsa_dec(&data, &priv_key);
}

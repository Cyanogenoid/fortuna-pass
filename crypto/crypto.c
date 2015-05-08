#include "crypto.h"

#include <avr/eeprom.h>
#include <stdlib.h>


static aes256_ctx_t aes_key;

void load_private_key(char* key) {
    // just save it, RSA is too slow
    aes256_init(key, &aes_key);
}

void* decrypt(void* buffer, uint16_t length_B) {
    size_t i;
    for (i = 0; i < length_B; i += 16) {
        aes256_dec(buffer+i, &aes_key);
    }
}

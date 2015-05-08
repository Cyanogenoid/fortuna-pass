#include "crypto.h"

#include <avr/eeprom.h>
#include <stdlib.h>

#include "os.h"
#include <avr/delay.h>

char eep_priv_key[PRIVATE_KEY_LENGTH_BYTE] EEMEM;
char eep_publ_key[PRIVATE_KEY_LENGTH_BYTE] EEMEM;
static rsa_privatekey_t priv_key;
static bigint_t priv_d;
static bigint_t publ_n;

void load_private_key(char* key) {
    // load key
    char* buffer = (char*) malloc(PRIVATE_KEY_LENGTH_BYTE);
    eeprom_read_block(buffer, eep_priv_key, PRIVATE_KEY_LENGTH_BYTE);
    char* publ_buffer = (char*) malloc(PRIVATE_KEY_LENGTH_BYTE);
    eeprom_read_block(publ_buffer, eep_publ_key, PRIVATE_KEY_LENGTH_BYTE);
    // decrypt with AES
    aes256_ctx_t context = {{0}};
    aes256_init(key, &context);
    size_t i;
    for (i = 0; i < PRIVATE_KEY_LENGTH_BYTE; i += 16) {
        aes256_dec(buffer+i, &context);
    }
    // make into bigints
    priv_d = (bigint_t) {
        .length_W = PRIVATE_KEY_LENGTH_BYTE,
        .info = 0,
        .wordv = buffer,
    };
    publ_n = (bigint_t) {
        .length_W = PRIVATE_KEY_LENGTH_BYTE,
        .info = 0,
        .wordv = publ_buffer,
    };
    bigint_adjust(&publ_n);
    bigint_adjust(&priv_d);
    // put into private key
    priv_key =  (rsa_privatekey_t) {
        .n = 1,
        .modulus = publ_n,
        .components = &priv_d,
    };
}

void* decrypt(void* buffer, size_t length_B) {
    bigint_t data = {
        .length_W = length_B,
        .info = 0,
        .wordv = (bigint_word_t*) buffer,
    };
    bigint_adjust(&data);

    rsa_dec(&data, &priv_key);
    display_string("\nPost: ");
    display_uint16(data.wordv[0]);
    _delay_ms(1000000);
    return data.wordv;
}

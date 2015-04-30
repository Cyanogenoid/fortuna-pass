#include "crypto.h"

#include <avr/eeprom.h>
#include <stdlib.h>

#include "os.h"

#define PRIVATE_KEY "\xed\x57\x7b\xc3\xd3\xfb\x83\x21\x2f\x58\x06\x94\x74\xb5\x0c\x85"
#define PRIVATE_KEY_LENGTH_BIT 2048
#define PRIVATE_KEY_LENGTH_BYTE PRIVATE_KEY_LENGTH_BIT/8
#define MODULUS 3233
#define MODULUS_RIGHT MODULUS & 0xFF
#define MODULUS_LEFT MODULUS >> 8

char eep_priv_key[PRIVATE_KEY_LENGTH_BYTE] EEMEM = PRIVATE_KEY;
static rsa_privatekey_t priv_key;
static bigint_t modulus = {
    .length_W = 2,
    .info = 0,
    .wordv = (bigint_t[]) {MODULUS_RIGHT, MODULUS_LEFT},
};
static bigint_t priv_d;

void load_private_key(char* key) {
    // // load key
    // char* buffer = (char*) malloc(PRIVATE_KEY_LENGTH_BYTE);
    // eeprom_read_block(buffer, eep_priv_key, PRIVATE_KEY_LENGTH_BYTE);
    // // decrypt with AES
    // aes256_ctx_t context = {{0}};
    // aes256_init(key, &context);
    // size_t i;
    // display_uint16(buffer[0]);
    // display_char('_');
    // display_uint16(buffer[1]);
    // for (i = 0; i < PRIVATE_KEY_LENGTH_BYTE; i += 16) {
    //     aes256_dec(buffer+i, &context);
    // }
    // display_char('-');
    // display_uint16(buffer[0]);
    // display_char('_');
    // display_uint16(buffer[1]);
    // // make into bigint
    // bigint_t local_priv_d = {
    //     .length_W = PRIVATE_KEY_LENGTH_BYTE,
    //     .wordv = buffer,
    // };
    // priv_d = local_priv_d;
    // bigint_adjust(&priv_d);
    // // put into private key
    // rsa_privatekey_t local_priv_key = {
    //     .n = 1,
    //     .modulus = modulus,
    //     .components = &priv_d,
    // };
    // priv_key = local_priv_key;
    // display_string("Loaded ");
    // // display_string(buffer);
    // display_string(" from EEPROM.\n");

    // // debug
    // strcpy(buffer, "secret");
    // display_uint16(buffer[0]);
    // display_char('_');
    // display_uint16(buffer[1]);
    // display_char('-');
    // for (i = 0; i < PRIVATE_KEY_LENGTH_BYTE; i += 16) {
    //     aes256_enc(buffer+i, &context);
    // }
    // display_uint16(buffer[0]);
    // display_char('_');
    // display_uint16(buffer[1]);
}

void decrypt(void* buffer, size_t length_B) {
    // bigint_t data = {
    //     .length_W = length_B,
    //     .wordv = buffer,
    // };
    // bigint_adjust(&data);
    // rsa_dec(&data, &priv_key);
}

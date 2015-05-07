#ifndef CRYPTO_H
#define CRYPTO_H


#include "avr-crypto-lib/bigint/bigint.h"
#include "avr-crypto-lib/rsa/rsa_basic.h"
#include "avr-crypto-lib/aes/aes.h"


#define AES_KEY "NN"
#define PRIVATE_KEY "hello"
#define PRIVATE_KEY_LENGTH_BIT 2048ULL
#define PRIVATE_KEY_LENGTH_BYTE PRIVATE_KEY_LENGTH_BIT/8
#define MODULUS 65537ULL
#define MODULUS_RIGHT MODULUS & 0xFF
#define MODULUS_MIDDLE MODULUS >> 8
#define MODULUS_LEFT MODULUS >> 16

char eep_priv_key[PRIVATE_KEY_LENGTH_BYTE];
bigint_t modulus;

/*
Load private key from EEPROM and decrypt it.
key: pointer to string used for AES decryption
*/
void load_private_key(char* key);

/*
Decrypt buffer with private key.
buffer: Pointer to buffer to be decrypted
length_B: length of the buffer in bytes
*/
void decrypt(void* buffer, size_t length_B);


#endif /* CRYPTO_H */

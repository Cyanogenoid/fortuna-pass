#ifndef CRYPTO_H
#define CRYPTO_H


#include "avr-crypto-lib/bigint/bigint.h"
#include "avr-crypto-lib/rsa/rsa_basic.h"
#include "avr-crypto-lib/aes/aes.h"

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

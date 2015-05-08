#ifndef CRYPTO_H
#define CRYPTO_H


#include "avr-crypto-lib/bigint/bigint.h"
#include "avr-crypto-lib/rsa/rsa_basic.h"
#include "avr-crypto-lib/aes/aes.h"


#define AES_KEY "NN"
#define PRIVATE_KEY "\x3D\x41\x95\xA4\xC3\x08\xCB\x34\x56\xA8\x8D\x3D\x51\x8C\xE4\x94\xDC\x72\xAD\xE5\x3E\xFE\x53\x34\x88\x40\x86\x79\x86\x94\xD2\x5F\x4D\xD2\x3C\x11\xF1\x57\xB0\x9B\x6E\x56\xCD\xFA\xBC\xD1\xF5\xFE\x73\xFF\x29\xDF\x53\xF5\xF0\x61\xDE\xE4\x91\x33\x97\xE0\x30\xA0\x81\xAE\x23\xAC\x42\xE9\xF3\xE8\x83\xC0\xCF\x5D\x5F\xBA\x19\xE9\xA1\x58\x85\x9E\x6D\xD1\xAD\xCC\x8F\xCC\xE2\xE1\xC2\xB0\xF7\x68\x83\x7C\x3B\x59\x07\xBE\xD2\x13\x08\x5E\x0C\x57\x52\x0F\xC8\x7E\x8E\x14\x04\x4D\x01\x9D\x45\x50\x1A\xAE\x25\x0D\xA7\xA0\xDD\xA0"
#define PRIVATE_KEY_LENGTH_BIT 1024ULL
#define PRIVATE_KEY_LENGTH_BYTE PRIVATE_KEY_LENGTH_BIT/8
// modulus = 65537
#define MODULUS_RIGHT 1
#define MODULUS_MIDDLE 0
#define MODULUS_LEFT 1

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
void* decrypt(void* buffer, size_t length_B);


#endif /* CRYPTO_H */
